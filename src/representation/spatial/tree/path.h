/*
 * path.h
 *
 *  Created on: Jun 25, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_SPATIAL_TREE_PATH_H_
#define SRC_REPRESENTATION_SPATIAL_TREE_PATH_H_

#include "pathelement.h"
#include "fixoverlongwires_impl.h"

namespace rhdl {
namespace spatial {


class Path {
	using Container = std::vector<PathElement>;

public:
	using iterator = Container::const_iterator;
	using const_iterator = iterator;

	Path(Container);
	virtual ~Path();

	iterator begin() const {return elements_.cbegin();}
	iterator end() const {return elements_.cend();}
	iterator cbegin() const {return elements_.cbegin();}
	iterator cend() const {return elements_.cend();}

	size_t size() const {return elements_.size();}
	blocks::index_t length() const {return length_;}

	const PathElement &operator[](size_t i) const {return elements_[i];}

	void evaluatePositions(
			const SegmentToPositionIndex &map,
			std::vector<PositionRating> &result) const;

	blocks::index_t freeLength(blocks::index_t start) const;
	blocks::index_t freeLength(blocks::index_t start, const_iterator &) const;

private:
	blocks::index_t nextRepeater(blocks::index_t start, const_iterator &) const;
	void find(blocks::index_t start, const_iterator &) const;

	Container elements_;
	blocks::index_t length_;
};

} /* namespace spatial */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_SPATIAL_TREE_PATH_H_ */
