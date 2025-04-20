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

void CObject::setDictionaryPtr(std::unique_ptr<Dictionary<const CObject>> dict)
{
	dict_ = std::move(dict);
	setMembers();
}

void CObject::setMembers()
{
	c_.content_.members = c_strings().data();
}

bool CObject::contains(const std::string &name) const
{
	assertInitialized();
	return dict_ -> contains(name);
}

bool CObject::contains(const char *name) const
{
	assertInitialized();
	return dict_ -> contains(name);
}

const CObject& CObject::at(const char *name) const
{
	if (!contains(name)) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}

	return dict_ -> at(name);
}

const CObject& CObject::at(const std::string &name) const
{
	if (!contains(name)) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}

	return dict_ -> at(name);
}

const Dictionary<const CObject>::CStrings& CObject::c_strings() const
{
	assertInitialized();
	return dict_ -> c_strings();
}

size_t CObject::size() const
{
	assertInitialized();
	return dict_ -> size();
}

void CObject::assertInitialized() const
{
	assert(dict_.get());
}

}
