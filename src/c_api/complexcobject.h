/*
 * cobjectimpl.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COMPLEXCOBJECT_H_
#define SRC_C_API_COMPLEXCOBJECT_H_

#include "cvalue.h"
#include "cvaluecontainer.h"
#include "util/dictionary/fcfsdictionary.h"


namespace rhdl {

template <bool OWNING=true>
class ComplexCObject : public CObject, public CValueContainer {
	using Super = CObject;
	using PT = typename std::conditional<
			OWNING, std::unique_ptr<const CObject>, const CObject *>::type;
	using Dict = dictionary::MutableDictionary<PT>;
	using DictPtr = std::unique_ptr<Dict>;

public:
	ComplexCObject(
			rhdl_type typeId, std::string name,
			DictPtr &&dict = std::make_unique<dictionary::FCFSDictionary<PT>>())
			: Super(typeId, name)
	{
		dict_ = std::move(dict);
		setDictionary();
	}

	ComplexCObject(ComplexCObject &&);

	virtual ~ComplexCObject() {};

	const CObject &add(PT &&member)
	{
		return *Super::add(*dict_, std::move(member));
	}

	const CObject &replace(PT &&member)
	{
		return *Super::replace(*dict_, std::move(member));
	}

protected:
	const CStrings &c_strings() const {return dict_ -> c_strings();}
	void clearDict() {dict_ -> clear();}

#if 0
	auto begin() {dict_ -> begin();}
	auto end() {dict_ -> end();}
#endif

private:
	const CObject &add(const CValue &cvalue) {
		return add(static_cast<PT>(&cvalue));
	}

	virtual const CObject &add_after_move(const CValue &cvalue) {
		return replace(static_cast<PT>(&cvalue));
	}

	void setDictionary() {
		Super::setDictionary(dictionary::DereferencingDictionaryAdapter<Dict>(*dict_));
	}

	DictPtr dict_;
};


template<bool OWNING>
inline ComplexCObject<OWNING>::ComplexCObject(
		ComplexCObject &&moved) :
				Super(std::move(moved)),
				dict_(std::move(moved.dict_))
{
	setDictionary();
}

}

#endif //SRC_C_API_COMPLEXCOBJECT_H_
