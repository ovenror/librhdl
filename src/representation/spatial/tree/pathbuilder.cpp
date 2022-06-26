/*
 * pathbuilder.cpp
 *
 *  Created on: Jun 25, 2022
 *      Author: ovenror
 */

#include "pathbuilder.h"
#include "path.h"

namespace rhdl {
namespace spatial {

PathBuilder::PathBuilder(const Connector &head)
		: head_(head), position_(0)
{}

PathBuilder::~PathBuilder() {}

PathBuilder &PathBuilder::operator+=(const Current &c)
{
	tail_.emplace_back(c, position_);
	const auto &recent = tail_.back();
	position_ = recent.endPos();
	auto &segment = recent.segment();
	head_ = recent.reverse() ? segment.frontConnector() : segment.backConnector();
	return *this;
}

PathBuilder PathBuilder::operator +(const Current &c) const
{
	PathBuilder result(*this);
	result += c;
	return result;
}

std::unique_ptr<Path> PathBuilder::build()
{
	std::vector<PathElement> pathv;
	std::move(tail_.begin(), tail_.end(), std::back_inserter(pathv));
	return std::make_unique<Path>(std::move(pathv));
}

bool PathBuilder::contains(const Segment &segment) const
{
	return std::any_of(tail_.begin(), tail_.end(),
		[&](const PathElement &e)
		{
			return &e.segment() == &segment;
		});
}

std::ostream& operator <<(std::ostream &os,	const PathBuilder &pb)
{
	pb.toStream(os);
	return os;
}

void PathBuilder::toStream(std::ostream &os) const
{
	for (auto &element : tail_) {
		os << element << std::endl;
	}
}

} /* namespace spatial */
} /* namespace rhdl */
