#include "../tree/wires.h"

#include "../tree/collectedwire.h"
#include "../tree/container.h"
#include "../tree/layer.h"
#include "../tree/singlewire.h"
#include "../tree/treemodel.h"

namespace rhdl {

using blocks::Blocks;

namespace spatial {

Wires::Wires(Container &owner, bool vertical, bool anchors)
	: Container(0), Crosser(vertical), owner_(owner), anchors_(anchors),
	  nextIndex_(0)
{}

Blocks::index_t Wires::xpos() const
{
	assert (!vertical_);
	return 0;
}

Blocks::index_t Wires::ypos() const
{
	assert (!vertical_);
	return position();
}

CollectedWire &Wires::make()
{
	return add(std::make_unique<CollectedWire>(*this, anchors_, *this, newIndex()));
}

CollectedWire &Wires::make(const std::shared_ptr<Connection> &connection)
{
	return add(std::make_unique<CollectedWire>(*this, connection, anchors_, *this, newIndex()));
}

CollectedWire &Wires::add(std::unique_ptr<CollectedWire> &&newWire)
{
	collection_.push_back(std::move(newWire));
	return *collection_.back();
}

std::unique_ptr<SingleWire> Wires::release(const Wire &wire)
{
	auto ipwire = collection_.begin();
	for (; ipwire != collection_.end() && ipwire -> get() != &wire; ++ipwire);
	assert (ipwire != collection_.end());
	std::unique_ptr<CollectedWire> &pCollectedWire = *ipwire;

	std::unique_ptr<SingleWire> pSingleWire = std::make_unique<SingleWire>(std::move(pCollectedWire));

	collection_.erase(ipwire);

	return pSingleWire;
}

CollectedWire &Wires::findOrMake(const std::shared_ptr<Connection> &connection)
{
	CollectedWire *result = findMax1(*connection);

	if (result)
		return *result;

	CollectedWire &wresult = make(connection);
	assert (wresult.isCrossConnected());

	return wresult;
}

CollectedWire &Wires::find(const Connection &connection) const
{
	CollectedWire *result = findMax1(connection);
	assert (result);
	return *result;
}

bool Wires::isConnectedTo(const Wire &w) const
{
	return std::any_of(collection_.begin(), collection_.end(), [&](auto &pwire) {return w.isConnected(*pwire);});
}

const Wire *Wires::getWireConnectedTo(const Wire &w) const
{
	for (const auto &pown : collection_) {
		if (pown -> isConnected(w))
			return pown.get();
	}

	return nullptr;
}

Wire *Wires::getWireConnectedTo(const Wire &w)
{
	const Wires *constThis = static_cast<const Wires *>(this);
	const Wire *const_result = constThis -> getWireConnectedTo(w);
	return const_cast<Wire *>(const_result);
}

CollectedWire *Wires::findMax1(const Connection &c) const
{
	CollectedWire *result = nullptr;

	for (auto &w : collection_) {
		if (c.isConnected(*w)) {
			assert (!result);
			result = w.get();
		}
	}

	return result;
}

std::vector<CollectedWire *> Wires::findConnected(const Connection &c) const
{
	std::vector <CollectedWire *> result;

	for (auto &w : collection_) {
		if (c.isConnected(*w))
			result.push_back(w.get());
	}

	return result;
}

void Wires::tryInsertCrosser(Wire &crosser)
{
	std::for_each(collection_.begin(), collection_.end(), [&](auto &pwire){pwire -> tryInsertCrosser_internal(crosser);});
}

void Wires::replaceCrosser(Wire &newCrosser)
{
	std::for_each(collection_.begin(), collection_.end(), [&](auto &pwire){pwire -> replaceCrosser_internal(newCrosser);});
}

bool Wires::mayBeCrossed(const Wire &wire, Blocks::index_t at) const
{
	return std::all_of(collection_.begin(), collection_.end(), [&](auto &pwire){return pwire -> mayBeCrossed_internal(wire, at);});
}

Blocks::index_t Wires::maxRelPos() const
{
	Blocks::index_t max = 0;

	for (auto &pwire : collection_) {
		max = std::max(max, pwire -> relativePosition_);
	}

	return max;
}

bool Wires::noNonAnchors() const
{
	return std::all_of(collection_.begin(), collection_.end(), [](auto &pwire){return pwire -> anchor_;});
}

std::vector<CollectedWire *> Wires::top() const
{
	return wiresAt(maxRelPos());
}

std::vector<CollectedWire *> Wires::bottom() const
{
	return wiresAt(0);
}

std::vector<CollectedWire *> Wires::wiresAt(Blocks::index_t position) const
{
	std::vector<CollectedWire *> result;

	for (auto &pwire : collection_) {
		if (pwire -> relativePosition_ == position && !pwire -> anchor_)
			result.push_back(pwire.get());
	}

	return result;
}


void Wires::applyToWires(std::function<void(Wire &)> f) const {
	for (auto &pwire : collection_) {
		f(*pwire);
	}
}

void Wires::computePositions()
{
	for (const auto &pwire : collection_)
		pwire -> dropHorizontalCollected();
}

const std::vector<const Wire *> Wires::debug_getWires() const
{
	std::vector<const Wire *> result;

	for (auto &pwire : collection_)
		result.push_back(pwire.get());

	return result;
}

unsigned int Wires::newIndex()
{
	assert (nextIndex_ >= collection_.size());
	return nextIndex_++;
}

bool Wires::dropCollision(const CollectedWire &dropped, Blocks::index_t at, const CollectedWire &other) const
{
	assert (!dropped.anchor_);
	assert (!other.anchor_);
	assert (!dropped.vertical());
	assert (!other.vertical());

	if (other.relativePosition_ != at)
		return false;

	return !dropped.hasDisjunctExtentsWith(other);
}

unsigned int Wires::numIsolatedUpperInputCrossings(const CollectedWire &wire) const
{
	unsigned int result = 0;

	assert (wire.collection() == this);
	assert (!vertical_);

	if (wire.anchor_)
		return 0;

	std::vector<const Wire *> upperLayerInputs;
	const Layer *layer = getLayer();

	if (!layer)
		upperLayerInputs = owner_.getModel().layers().begin() -> inputs();
	else
		upperLayerInputs = layer -> upperLayerInputs();

	for (const Wire *input : upperLayerInputs) {
		assert (input -> hasPosition());

		if (input -> isInCrossingRangeOf(wire) && !input -> isConnected(wire))
			++result;
	}

	return result;
}

}
}
