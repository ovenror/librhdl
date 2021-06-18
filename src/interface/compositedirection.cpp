/*
 * compositedirection.cpp
 *
 *  Created on: Sep 9, 2021
 *      Author: ovenror
 */

#include <interface/compositedirection.h>

namespace rhdl {

CompositeDirection::CompositeDirection(int code)
	: code_(code)
{}

CompositeDirection::CompositeDirection(bool out, bool in)
	: CompositeDirection(out ? vout() : 0 | in ? vin() : 0)
{}

CompositeDirection::CompositeDirection()
	: CompositeDirection(false, false)
{}

CompositeDirection::CompositeDirection(SingleDirection dir)
	: CompositeDirection(dir == SingleDirection::OUT, dir == SingleDirection::IN)
{}

bool CompositeDirection::operator ==(const CompositeDirection &rhs) const
{
	return code_ == rhs.code_;
}

void CompositeDirection::decode(std::ostream &os)
{
	if (in())
		if (out())
			os << "MIXED";
		else
			os << "IN";
	else
		if (out())
			os << "OUT";
		else
			os << "FREE";
}


std::ostream& operator <<(std::ostream &os, CompositeDirection dir)
{
	dir.decode(os);
	os << "(" << dir.code_ << ")";
 	return os;
}

CompositeDirection& CompositeDirection::operator |=(
		const CompositeDirection &rhs)
{
	code_ |= rhs.code_;
	return *this;
}

CompositeDirection CompositeDirection::operator |(
		const CompositeDirection &rhs) const
{
	return CompositeDirection(code_ | rhs.code_);
}

CompositeDirection& CompositeDirection::operator &=(
const CompositeDirection &rhs)
{
	code_ &= rhs.code_;
	return *this;
}

CompositeDirection CompositeDirection::operator &(
const CompositeDirection &rhs) const
{
	return CompositeDirection(code_ & rhs.code_);
}

CompositeDirection CompositeDirection::complement() const
{
	return CompositeDirection(in(), out());
}

CompositeDirection::operator SingleDirection() const
{
	 assert(single());
	 return in() ? SingleDirection::IN : SingleDirection::OUT;
}

} /* namespace rhdl */
