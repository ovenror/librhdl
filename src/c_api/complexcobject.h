/*
 * cobjectimpl.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COMPLEXCOBJECT_H_
#define SRC_C_API_COMPLEXCOBJECT_H_

#include "cvaluecontainer.h"
#include "cvalue.h"

#include "util/lexicalpointingdictionary.h"

namespace rhdl {

class CValue;

template <bool OWNING=true>
class ComplexCObject : public CValueContainer {
	using Super = CValueContainer;
	using PT = typename std::conditional<
			OWNING, std::unique_ptr<const CObject>, const CObject*>::type;

public:
	ComplexCObject(rhdl_type typeId, std::string name) : Super(typeId, name)
	{
		Super::setMembers();
	}

	ComplexCObject(ComplexCObject &&);

	virtual ~ComplexCObject() {};

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	const CObject &at(const std::string &name) const override;
	const CObject &at(const char *name) const override;

	const CObject &add(PT member)
	{
		Super::updateContainerFor(*member, *this);

		auto ptr = dict_.add(std::move(member));

		if (!ptr) {
			throw ConstructionException(Errorcode::E_MEMBER_EXISTS, member -> name());
		}

		Super::setMembers();
		return *ptr;
	}

	const CObject &replace(PT member)
	{
		Super::updateContainerFor(*member, *this);

		auto ptr = dict_.replace(std::move(member));

		if (!ptr) {
			throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER, member -> name());
		}

		Super::setMembers();
		return *ptr;
	}

	std::size_t size() const override {return dict_.size();}

	const std::vector<const char*> &c_strings() const override {return dict_.c_strings();}

protected:
	void clearDict() {dict_.clear();}

	auto begin() {dict_.begin();}
	auto end() {dict_.end();}

private:
	const CObject &add(const CValue &cvalue) {
		return add(static_cast<PT>(&cvalue));
	}

	virtual const CObject &add_after_move(const CValue &cvalue) {
		return replace(static_cast<PT>(&cvalue));
	}

	LexicalPointingDictionary<CObject, OWNING> dict_;
};

template<bool OWNING>
inline const CObject& ComplexCObject<OWNING>::at(const std::string &name) const
{
	if (!dict_.contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);

	return dict_.at(name);
}

template<bool OWNING>
inline ComplexCObject<OWNING>::ComplexCObject(
		ComplexCObject &&moved) :
				Super(std::move(moved)),
				dict_(std::move(moved.dict_))
{}

//TODO: These should go into a non-templated class
template<bool OWNING>
inline bool ComplexCObject<OWNING>::contains(
		const std::string &name) const
{
	return dict_.contains(name);
}

template<bool OWNING>
inline bool ComplexCObject<OWNING>::contains(const char *name) const
{
	return dict_.contains(name);
}

template<bool OWNING>
inline const CObject& ComplexCObject<OWNING>::at(const char *name) const
{
	if (!dict_.contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);

	return dict_.at(name);
}

}

#endif //SRC_C_API_COMPLEXCOBJECT_H_
