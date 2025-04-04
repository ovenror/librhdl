/*
 * stringconversion.cpp
 *
 *  Created on: Apr 11, 2025
 *      Author: js
 */

#include "stringconversion.h"
#include "cobject.h"

#include <rhdl/construction/c/types.h>

#include "interface/direction.h"
#include "representation/representations.h"

#include <sstream>
#include <cassert>

namespace rhdl {

template <class VALUE_TYPE>
std::string to_string(VALUE_TYPE value)
{
	return (std::stringstream() << value).str();
}

template <>
std::string to_string<const CObject *>(const CObject *obj)
{
	return (std::stringstream() << "* " << (obj ? obj -> fqn() : "<null>")).str();
}

template <>
std::string to_string<const CObject &>(const CObject &obj)
{
	return (std::stringstream() << "& " + obj.fqn()).str();
}

template <>
std::string to_string<rhdl_direction>(rhdl_direction value)
{
	return (std::stringstream() << SingleDirection(value)).str();
}

template <>
std::string to_string<rhdl_reptype>(rhdl_reptype value)
{
	return (std::stringstream() << representations.objects()[value].name()).str();
}

//required for linkage, should never be called
template <>
std::string to_string<const std::string &>(const std::string &value)
{
	assert(0);
	return value;
}
template std::string to_string<const std::string &>(const std::string &);

template std::string to_string<rhdl_direction>(rhdl_direction);
template std::string to_string<rhdl_reptype>(rhdl_reptype);
template std::string to_string<const CObject *>(const CObject *);
template std::string to_string<const CObject &>(const CObject &);

} /* namespace rhdl */
