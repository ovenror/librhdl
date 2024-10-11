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

template <
		class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID, bool OWNING=true,
		class TYPED_BASE = CObject, class VALUE=CObject>
class CObjectImpl : public TypedCObject<
		CRTP, Typed_C_Struct, TYPE_ID, TYPED_BASE> {
	using Super = TypedCObject<CRTP, Typed_C_Struct, TYPE_ID, TYPED_BASE>;
	using PT = typename std::conditional<
			OWNING, std::unique_ptr<const VALUE>, const VALUE*>::type;

public:
	CObjectImpl(std::string name) : Super(name)
	{
		Super::setMembers(c_strings());
	}
	CObjectImpl(CObjectImpl &&);

	virtual ~CObjectImpl() {};

	const CObject &at(const std::string &name) const override;
	const CObject &at(const char *name) const override;

	const CObject &add(PT member)
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

protected:
	void clearDict() {dict_.clear();}

	auto begin() {dict_.begin();}
	auto end() {dict_.end();}

	LexicalPointingDictionary<VALUE, OWNING> dict_;
};

template<
		class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID, bool OWNING,
		class TYPED_BASE, class VALUE>
inline const CObject& CObjectImpl<
		CRTP, Typed_C_Struct, TYPE_ID, OWNING, TYPED_BASE, VALUE>::at(
				const std::string &name) const
{
	try {
		return dict_.at(name);
	}
	catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
	}
}

template<class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID,
		bool OWNING, class TYPED_BASE, class VALUE>
inline CObjectImpl<CRTP, Typed_C_Struct, TYPE_ID, OWNING, TYPED_BASE, VALUE>::CObjectImpl(
		CObjectImpl &&moved) :
				Super(std::move(moved)),
				dict_(std::move(moved.dict_))
{}

template<
		class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID, bool OWNING,
		class TYPED_BASE, class VALUE>
inline const CObject& CObjectImpl<
		CRTP, Typed_C_Struct, TYPE_ID, OWNING, TYPED_BASE, VALUE>::at(
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
