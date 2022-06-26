#ifndef SHAREDSEGMENT_H
#define SHAREDSEGMENT_H

#include "representation/blocks/types.h"

#include "boost/iterator/transform_iterator.hpp"

#include <tuple>
#include <vector>
#include <functional>
#include <iostream>
#include "../tree/segment.h"

namespace rhdl {
namespace spatial {

class Wire;

class Connector
{
public:
	Connector(blocks::index_t xpos, blocks::index_t ypos);

	bool terminal() {return terminal_;}

	using Super = std::function<const Current(const AtomicCurrent &)>;
	using SuperCurrentIterator = boost::transform_iterator<Super, std::vector<AtomicCurrent>::const_iterator>;
	using SuperCurrentIterable = Iterable<SuperCurrentIterator>;

	const std::vector<AtomicCurrent> &connected() const {return connected_;}
	SuperCurrentIterable superConnected() const;

	blocks::index_t getPositionOn(const Wire &w) const;

	void addIncoming(AtomicSegment &segment);
	void addOutgoing(AtomicSegment &segment);

	bool terminal() const;
	AtomicSegment *straightPartner(const AtomicSegment &segment) const;

	std::pair<blocks::index_t, blocks::index_t> position() const {return {xpos_, ypos_};}

protected:
	void add(AtomicCurrent &&c);

private:
	void checkPathTerminality(const AtomicCurrent &c);
	bool breaksTerminality(const AtomicCurrent &c) const;

	std::vector<AtomicCurrent> connected_;
	blocks::index_t xpos_;
	blocks::index_t ypos_;
	bool terminal_;
};

std::ostream &operator<<(std::ostream &os, const Connector &segment);


}}

#endif // SHAREDSEGMENT_H
