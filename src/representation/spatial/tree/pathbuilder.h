/*
 * pathbuilder.h
 *
 *  Created on: Jun 25, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_SPATIAL_TREE_PATHBUILDER_H_
#define SRC_REPRESENTATION_SPATIAL_TREE_PATHBUILDER_H_

#include "representation/blocks/types.h"
#include "pathelement.h"

#include <deque>
#include <functional>
#include <memory>
#include <ostream>

namespace rhdl {
namespace spatial {

class Connector;
class Path;

class PathBuilder {
public:
	PathBuilder(const Connector &);
	PathBuilder(const PathBuilder &) = default;

	virtual ~PathBuilder();

	PathBuilder operator+(const Current &) const;
	PathBuilder &operator+=(const Current &);

	const Connector &head() const {return head_;}
	size_t size() const {return tail_.size();}
	bool contains(const Segment &) const;

	std::unique_ptr<Path> build();
	void toStream(std::ostream &) const;

private:
	std::deque<PathElement> tail_;
	std::reference_wrapper<const Connector> head_;
	blocks::index_t position_;
};

std::ostream &operator<<(std::ostream &, const PathBuilder &);

} /* namespace spatial */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_SPATIAL_TREE_PATHBUILDER_H_ */
