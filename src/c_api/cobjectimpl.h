/*
 * cobjectimpl.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COBJECTIMPL_H_
#define SRC_C_API_COBJECTIMPL_H_

#include "typedcobject.h"
#include "util/lexicalpointingdictionary.h"

namespace rhdl {

template <class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
class CObjectImpl : public TypedCObject<CRTP, Typed_C_Struct, TYPE_ID> {
	using Super = TypedCObject<CRTP, Typed_C_Struct, TYPE_ID>;

public:
	CObjectImpl(std::string name) : TypedCObject<CRTP, Typed_C_Struct, TYPE_ID>(name)
	{
		Super::setMembers(c_strings());
	}

	virtual ~CObjectImpl() {};

	CObject &at(const std::string &name) override;
	const CObject& at(const std::string &name) const override;
	const CObject &at(const char *name) const override;

	const CObject &add(std::unique_ptr<CObject> member)
	{
		auto ptr = dict_.add(std::move(member));

		if (!ptr) {
			throw ConstructionException(Errorcode::E_MEMBER_EXISTS, member -> name());
		}

		Super::setMembers(c_strings());
		return *ptr;
	}

	std::size_t size() const override {return dict_.size();}

	const std::vector<const char*> &c_strings() const override {return dict_.c_strings();}

private:
	LexicalPointingDictionary<CObject> dict_;
};

template<class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
inline CObject& CObjectImpl<CRTP, Typed_C_Struct, TYPE_ID>::at(
		const std::string &name)
{
	try {
		return dict_.at(name);
	}
	catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}
}

template<class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
inline const CObject& CObjectImpl<CRTP, Typed_C_Struct, TYPE_ID>::at(
		const std::string &name) const
{
	try {
		return dict_.at(name);
	}
	catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}
}

template<class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
inline const CObject& CObjectImpl<CRTP, Typed_C_Struct, TYPE_ID>::at(
		const char *name) const
{
	try {
		return dict_.at(name);
	}
	catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}
}

}

#endif //SRC_C_API_COBJECTIMPL_H_
