/*
 * pathelement.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_SPATIAL_TREE_PATHELEMENT_H_
#define SRC_REPRESENTATION_SPATIAL_TREE_PATHELEMENT_H_

#include "segment.h"
#include "representation/blocks/types.h"

#include <functional>
#include <optional>

namespace rhdl::spatial {

class PathElement {
public:
	PathElement(Segment &segment, bool reverse, blocks::index_t position);
	PathElement(const Current &current, blocks::index_t position);
	PathElement(const PathElement &) = default;

	virtual ~PathElement();

	PathElement &operator=(const PathElement &) = default;
	operator Segment &() const {return segment();}
	operator Current() const {return current();}

	Segment &segment() const {return segment_;}
	Current current() const {return Current(&segment_.get(), reverse_);}

	bool reverse() const {return reverse_;}
	blocks::index_t startPos() const {return startPosInPath_;}
	blocks::index_t endPos() const {return endPosInPath_;}

	blocks::index_t nextRepeater() const;
	blocks::index_t nextRepeater(blocks::index_t pos) const;

private:
	inline blocks::index_t nextRepeater_internal(blocks::index_t relPos) const;

	std::reference_wrapper<Segment> segment_;
	bool reverse_;
	blocks::index_t startPosInPath_;
	blocks::index_t endPosInPath_;
};

std::ostream &operator<<(std::ostream &, const PathElement &);


} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_SPATIAL_TREE_PATHELEMENT_H_ */
