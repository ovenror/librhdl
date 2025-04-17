/*
 * cobject.cpp
 *
 *  Created on: Apr 11, 2025
 *      Author: js
 */

#include "cobject.h"
#include "rootnamespace.h"
#include <cassert>

namespace rhdl {

CObject::CObject(rhdl_type typeId, std::string name)
		: c_(*this), name_(name) {
	c_.content_.type = typeId;
	c_.content_.name = name_.c_str();
	c_.content_.container = c_ptr(container_);
}

CObject::CObject(CObject &&moved) :
		c_(std::move(moved.c_), *this),
		name_(std::move(moved.name_))
{
	c_.content_.name = name_.c_str();

	assert(c_.content_.container == c_ptr(container()));
}

CObject::~CObject() {}

const std::string CObject::fqn() const
{
	std::string result = name_;
	auto iter = container_;

	for (; iter != &rootNamespace && iter; iter = iter -> container_)
	{
		result = iter -> name_ + "." + result;
	}

	assert (iter == &rootNamespace);
	return result;
}

CObject::operator const char*() const
{
	return static_cast<const std::string &>(*this).c_str();
}

void CObject::setMembers()
{
	auto members = c_strings().data();
	c_.content_.members = members;
	setMembers_internal(members);
}

}

