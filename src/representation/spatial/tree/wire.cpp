#include "../tree/wire.h"
#include "node.h"
#include <algorithm>
#include <limits>
#include "../tree/connection.h"
#include "../tree/connector.h"
#include "../tree/container.h"
#include "../tree/layer.h"
#include "../tree/node.h"
#include "../tree/nodegroup.h"
#include "../tree/uniquesegment.h"
#include "../tree/wires.h"

namespace rhdl {
namespace spatial {

using blocks::Blocks;
using blocks::Block;

Wire::Wire(Container &owner, bool anchor, unsigned int index)
	:
	  Element(index), owner_(owner), anchor_(anchor), has_extents_(false),
	  relativePosition_(0), start_(0), end_(0), connectors_(ConnectorLess(*this))
{
	connection_ = std::shared_ptr<Connection>(new Connection(*this));
	assert(isConnected(*connection_));
}

Wire::Wire(Wire &&dying, Container &newOwner) :
	Element(dying), owner_(newOwner), anchor_(dying.anchor_),
	has_extents_(dying.has_extents_),
	relativePosition_(dying.relativePosition_), start_(dying.start_),
	end_(dying.end_), connectors_(ConnectorLess(*this))
{
	connection_ = std::shared_ptr<Connection>(new Connection(*this));
	connect(dying.connection_);
	sorted_crossers_ = std::move(dying.sorted_crossers_);
}

Wire::~Wire()
{
	connection_ -> reject(*this);
}

Node *Wire::getNode() const
{
	return owner_.getNode();
}

NodeGroup *Wire::getNodeGroup() const
{
	return owner_.getNodeGroup();
}

Layer *Wire::getLayer() const
{
	return owner_.getLayer();
}

void Wire::connect(const std::shared_ptr<Connection> &to)
{
	const std::shared_ptr<Connection> oldconn = connection_;
	to -> accept(*this);

	for (Wire *w : *oldconn)
	{
		w -> connection_ = to;
		assert(to -> isConnected(*w));
	}
}

void Wire::connect(const Wire &w)
{
	connect(w.connection_);
}

void Wire::disconnect()
{
	connection_ -> reject(*this);
	connection_ = std::shared_ptr<Connection>(new Connection(*this));
}

bool Wire::isConnected(const Connection &c) const
{
	return c.isConnected(*this);
}

bool Wire::isConnected(const Wire &w) const
{
	return connection_ -> isConnected(w);
}

bool Wire::isConnected(const Wires &c) const
{
	return c.isConnectedTo(*this);
}

const Node *Wire::getInverter() const
{
	Node *n = getNode();

	if (n && n -> invert_)
		return n;
	else
		return nullptr;
}

bool Wire::isInverterInput() const
{
	const Node *n = getInverter();    
	assert (!n || !n -> backwards());
	return n && isInputOf(n);
}

bool Wire::isInverterOutput() const
{
	const Node *n = getInverter();
	assert (!n || !n -> backwards());
	return n && isOutputOf(n);
}

bool Wire::isInputOfInverter(const Node *n) const
{
	assert (n && !n -> backwards());
	return n -> invert_ && isInputOf(n);
}

bool Wire::isOutputOfInverter(const Node *n) const
{
	assert (n && !n -> backwards());
	return n -> invert_ && isOutputOf(n);
}

bool Wire::isInputOf(const Node *n) const
{
	assert (n);
	return this == &n -> input_;
}

bool Wire::isOutputOf(const Node *n) const
{
	assert (n);
	return this == &n -> output_;
}

void Wire::computeAbsolutePosition()
{
	computeAbsolutePosition(vertical() ? owner_.xpos() : owner_.ypos());
}

void Wire::computeAbsolutePosition(Blocks::index_t offset)
{
	assert (offset < 16*9*2);
	assert (relativePosition_ < 16*9*2);

	setPosition(relativePosition_ + offset);
}

void Wire::setPosition(Blocks::index_t position)
{    
	Element::setPosition(position);

	//std::cerr << "pos " << *this << std::endl;

	for (Crosser *crosser : getCrossers()) {
		const auto &crosser_crossers = crosser -> getCrossers();

		Crosser &cthis = asCrosser();
		assert (crosser_crossers.find(&cthis) != crosser_crossers.end());

		crosser -> tryInsertCrosser(*this);
	}
}

bool Wire::hasDisjunctExtentsWith(const Wire &other) const
{
	if (other.end_ < start_)
		return true;

	if (other.start_ > end_)
		return true;

	return false;
}

bool Wire::hasOverlappingExtentsWith(const Wire &other) const
{
	if (other.end_ <= start_)
		return false;

	if (other.start_ >= end_)
		return false;

	return true;
}

std::vector<Wire::Range> Wire::freeRanges(Blocks::index_t minSize) const
{
	std::vector<Wire::Range> result;

	assert (!has_extents_);
	assert (!anchor_);

	Blocks::index_t start = start_;

	for (const Wire *w : sorted_crossers_) {
		Blocks::index_t end = w -> position();
		if (end - start >= minSize)
			result.push_back({start, end});
	}

	return result;
}

void Wire::tryInsertCrosser_internal(Wire &crosser)
{
	assert (!has_extents_);

	Blocks::index_t where = crosser.position();

	if (crosser.has_extents_ && hasPosition()) {
		if (position() < crosser.start_ - 1)
			return;

		if (position() > crosser.end_)
			return;
	}

	if (!mayBeCrossed_internal(crosser, where)) {
		std::cerr << "***COLLISION***" << std::endl;
		std::cerr << "  this     : " << *this << std::endl;
		std::cerr << "  crosser  : " << crosser << std::endl;
		std::cerr << "  existing : " << **sorted_crossers_.find(crosser.position()) << std::endl;
		assert (0);
	}

	auto result = sorted_crossers_.insert(&crosser);

	const Wire &existing = **result.first;

	if (
			!result.second &&
			isConnected(crosser) &&
			(
				existing.anchor_ ||
				!isConnected(existing)
			)

	) {
		assert (existing.anchor_ || crosser.anchor_);
		sorted_crossers_.erase(result.first);
		result = sorted_crossers_.insert(&crosser);
		assert(result.second);
	}
}

void Wire::replaceCrosser_internal(Wire &newCrosser)
{
	auto iOldCrosser = sorted_crossers_.find(&newCrosser);
	assert (iOldCrosser != sorted_crossers_.end());
	Wire &oldCrosser = **iOldCrosser;
	assert (oldCrosser.isConnected(newCrosser));

	sorted_crossers_.erase(iOldCrosser);
	sorted_crossers_.insert(&newCrosser);
}

bool Wire::mayBeCrossed_internal(const Wire &by, Blocks::index_t at) const
{
	assert (vertical() != by.vertical());

	auto iter = sorted_crossers_.find(at);

	assert (*iter != &by);

	if (iter == sorted_crossers_.end())
		return true;

	const Wire &existing = **iter;

	return existing.isConnected(by) || by.anchor_ || existing.anchor_;
}

bool Wire::mayCross(const Crosser &crosser, Blocks::index_t at) const
{
	return crosser.mayBeCrossed(*this, at);
}

bool Wire::isCrosser(const Wire &w) const
{
	return asCrosser().isCrosser(w.asCrosser());
}

bool Wire::isCrossConnected() const
{
	const auto &crossers = getCrossers();

	for (Crosser *crosser : crossers) {
		if (crosser -> isConnectedTo(*this))
			return true;
	}

	return false;
}

bool Wire::isCrossConnected(const Wire &w) const
{
	return isCrosser(w) && isConnected(w);
}

bool Wire::isImmediatelyConnected(const Wire &w) const
{
	return occupiesSameXY(w) && isConnected(w);
}

bool Wire::isInCrossingRangeOf(const Wire &w) const
{
	assert(w.has_extents_);
	assert(vertical() != w.vertical());

	return position() >= w.start_ && position() < w.end_;
}

std::vector<Wire *> Wire::wiresCrossConnectedAt(Blocks::index_t pos) const
{
	std::vector<Wire *> result;

	for (Crosser *c : getCrossers()) {
		Wire *connected = c -> getWireConnectedTo(*this);

		if (!connected)
			continue;

		if (connected && connected -> position() == pos)
			result.push_back(connected);
	}

	return result;
}


bool Wire::occupiesSameXY(const Wire &w) const
{
	assert (has_extents_ && w.has_extents_);

	Blocks::index_t pos = position();
	Blocks::index_t wpos = w.position();

	if (vertical() == w.vertical()) {
		if (pos != wpos)
			return false;

		return hasOverlappingExtentsWith(w);
	}
	else {
		return isInCrossingRangeOf(w) && w.isInCrossingRangeOf(*this);
	}
}

std::vector<Wire *> Wire::wiresConnectedAt(Blocks::index_t pos) const
{
	std::vector<Wire *> result;

	assert (pos >= start_ && pos < end_);

	for (Wire *w : *connection_)
	{
		if (w == this || !isImmediatelyConnected(*w))
			continue;

		if (w -> vertical() == vertical()) {
			if (
					(pos != start_ && pos != end_ - 1) ||
					(pos != w -> start_ && pos != w -> end_ - 1))
				continue;
		}
		else {
			if (w -> position() != pos)
				continue;
		}

		result.push_back(w);
	}

	return result;
}

bool Wire::isCrossedBy(const Wire &w) const
{
	return isCrosser(w) && w.isInCrossingRangeOf(*this);
}

bool Wire::crosses(const Wire &w) const
{
	return w.isCrossedBy(*this);
}

void Wire::computeExtents()
{
	//std::cerr << "a.ext " << *this << std::endl;

	if (sorted_crossers_.empty())
		std::cerr << *this << " has no crossers!" << std::endl;

	assert (!sorted_crossers_.empty());

	auto front(sorted_crossers_.begin());
	for (; front != sorted_crossers_.end() && !isConnected(**front); ++front);

	assert (front != sorted_crossers_.end());

	sorted_crossers_.erase(sorted_crossers_.begin(), front);
	start_ = (*front) -> position();

	auto rback(sorted_crossers_.rbegin());
	for (; rback != sorted_crossers_.rend() && !isConnected(**rback); ++rback);

	assert (rback != sorted_crossers_.rend());

	auto back = (++rback).base();
	end_ = (*back) -> position() + 1;
	sorted_crossers_.erase(++back, sorted_crossers_.end());
	assert (end_ - start_ > 0);

	has_extents_ = true;

	//std::cerr << "p.ext " << *this << std::endl;
}

void Wire::tryBecomeAnchor()
{
	assert (has_extents_);

	if (sorted_crossers_.size() == 1) {
		assert (end_ - start_ == 1);
		anchor_ = true;
	}
}

void Wire::toBlocks(Blocks::Cuboid b) const
{
	//std::cerr << "blk " << *this << std::endl;

	if (anchor_) {
		return;
	}

	/*
	 * TODO: Use segments
	 */
	Blocks::Wall seg = segment(b); // not that one
	Blocks::index_t lastConnection = start_;

	for (auto *pcrosser : sorted_crossers_) {
		const Wire &crosser = *pcrosser;

		if (!isConnected(crosser))
			continue;

		Blocks::index_t currentConnection = crosser.position();
		assert (currentConnection < end_);
		assert (currentConnection >= lastConnection || currentConnection == start_);
		Blocks::index_t blocks_between = currentConnection - lastConnection;

		if (crosser.anchor_)
			if (vertical()) //purely cosmetic
				continue;

		//if (blocks_between == 1) {
		//    blocks_connected(seg, lastConnection, 2);
		//}
		//else {
			blocks_isolated(seg, lastConnection, blocks_between);
			blocks_connected(seg, currentConnection, 1);
		//}

		lastConnection = currentConnection + 1;
	}

	blocks_isolated(seg, lastConnection, end_ - lastConnection);

	placeRepeaters(b);
}

void Wire::createSegments()
{
	assert (uniqueSegments_.empty());

	auto iw = sorted_crossers_.begin();
	const Wire &w = **iw;
	++iw;

	assert(w.position() == start_);

	Blocks::index_t lastPos = start_;
	Connector *lastSharedSegment = addConnector(lastPos);

	for (; iw != sorted_crossers_.end(); ++iw) {
		const Wire &w = **iw;

		if (!isConnected(w))
			continue;

		Blocks::index_t nextPos = w.position();
		assert (vertical() || nextPos >= lastPos + 2);

		Connector *nextSharedSegment = addConnector(nextPos);
		addSegment(lastPos, nextPos, lastSharedSegment, nextSharedSegment);

		lastPos = nextPos;
		lastSharedSegment = nextSharedSegment;
	}

	assert (lastPos == end_ - 1);
}

bool Wire::hasSegments() const
{
	return !connectors_.empty();
}

const Connector &Wire::front() const
{
	return getConnectorAt(start_);
}

const Connector &Wire::back() const
{
	return getConnectorAt(end_ - 1);
}

UniqueSegment &Wire::addSegment(
		Blocks::index_t start, Blocks::index_t end,
		Connector *startSeg, Connector *endSeg)
{
	auto ptr = std::make_unique<UniqueSegment>(*this, start, end, *startSeg, *endSeg);
	UniqueSegment &result = *ptr;
	uniqueSegments_.push_back(std::move(ptr));
	return result;
}

Connector *Wire::addConnector(Blocks::index_t pos)
{
	auto iseg = connectors_.find(pos);

	//std::cerr << *this << " searching for SharedSegment at " << pos << std::endl;
	if (iseg != connectors_.end()) {
		//std::cerr << "  found it" << std::endl;

		for (Wire *w : wiresConnectedAt(pos)) {
			//std::cerr << "  checking " << *w << std::endl;;
			assert (w -> connectors_.find(*iseg) != w -> connectors_.end());
		}

		return iseg -> get();
	}

	Blocks::index_t xpos = vertical() ? position() : pos;
	Blocks::index_t ypos = vertical() ? pos : position();
	std::shared_ptr<Connector> segPtr = nullptr;

	//std::cerr << "  create" << std::endl;
	segPtr = std::make_shared<Connector>(xpos, ypos);
	connectors_.insert(segPtr);

	for (Wire *w : wiresConnectedAt(pos)) {
		//std::cerr << "  add to " << *w << std::endl;;
		w -> connectors_.insert(segPtr);
	}

	return segPtr.get();
}

const Connector &Wire::onlyConnector() const
{
	assert (connectors_.size() == 1);
	return **connectors_.begin();
}

Connector &Wire::getConnectorAt(Blocks::index_t pos) const
{
	assert (!connectors_.empty());
	auto iseg = connectors_.find(pos);
	assert (iseg != connectors_.end());
	return **iseg;
}

Blocks::Line Wire::line(Blocks::Wall segment, Blocks::index_t height, Blocks::index_t position, Blocks::index_t length)
{
	return Blocks::slice1(segment, {height, position}, (Axis) 1, length);
}

Blocks::Wall Wire::segment(Blocks::Cuboid &blocks) const
{
	return Blocks::slice2(
				blocks, {0, vertical() ? position() : 0, vertical() ? 0 : position()},
				(Axis) (vertical() ? 1 : 2), {0, 0});
}

void Wire::placeRepeater(const Repeater &r, blocks::Blocks::Cuboid b) const
{
	using blocks::Direction;
	using blocks::FORWARD;
	using blocks::RIGHT;

	Direction orientation = vertical() ? FORWARD : RIGHT;

	if (r.backwards)
		orientation = (Direction) (orientation | 2);

	segment(b)[vertical() ? 1 : 3][r.position] = Block(Block::REPEATER, orientation);
}

void Wire::blocks_isolated(Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length) const
{
	if (!length)
		return;

	blocks(vertical() ? -1 : 1, line_segment, position, length);
}

void Wire::blocks_connected(Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length) const
{
	blocks(0, line_segment, position, length);
}

void Wire::blocks(int height_offset, Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length)
{
	Blocks::fill(line(line_segment, 1 + height_offset, position, length), Block::OPAQUE);
	Blocks::fill(line(line_segment, 2 + height_offset, position, length), Block::REDSTONE);
}

std::ostream &operator<<(std::ostream &os, const Wire &cw)
{
	Wire &w = const_cast<Wire &>(cw);

	os << "Wire(" << &w << ")@" << w.owner_;
	os << " " << (w.vertical()?"-":"|");
	os << " rp:" << w.relativePosition_;
	os << " p:";

	if (w.hasPosition())
		os << w.position();
	else
		os << "?";

	os << " " << w.start_ << "--" << w.end_;

	os << " c:";
	for (const Wire *c : w.sorted_crossers_) {
		if (w.isConnected(*c))
			os << "*";
		else
			os << "+";
		os << c -> position()<< ",";
	}

	os << " ac:";
	for (const Crosser *c : w.getCrossers()) {
		for (const Wire *w : c -> debug_getWires()) {
			if (w -> hasPosition())
				os << w -> position();
			else
				os << "?";
			os << ",";
		}
		os << "; ";
	}

	if (w.anchor_)
		os << " A";

	return os;
}

Wire::ConnectorLess::ConnectorLess(const Wire &This)
	: this_(This)
{}

bool Wire::ConnectorLess::operator()(const std::shared_ptr<Connector> &lhs, const std::shared_ptr<Connector> &rhs) const
{
	assert (lhs.get() == rhs.get() || lhs -> getPositionOn(this_) != rhs -> getPositionOn(this_));
	return lhs -> getPositionOn(this_) < rhs -> getPositionOn(this_);
}

bool Wire::ConnectorLess::operator()(const std::shared_ptr<Connector> &lhs, Blocks::index_t rhs) const
{
	return lhs -> getPositionOn(this_) < rhs;
}

bool Wire::ConnectorLess::operator()(Blocks::index_t lhs, const std::shared_ptr<Connector> &rhs) const
{
	return lhs < rhs -> getPositionOn(this_);
}

void Wire::addRepeater(Blocks::index_t position, bool backwards)
{
	assert (position >= start_ && position < end_);

	Repeater r{position, backwards};
	repeaters_.emplace(std::make_unique<Repeater>(std::move(r)));
}

void Wire::placeRepeaters(blocks::Blocks::Cuboid b) const
{
	for (const auto &repeater : repeaters_) {
		placeRepeater(*repeater, b);
	}
}

}
}

