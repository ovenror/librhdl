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

#include "util/lexicaldictionary.h"
#include "util/dictionaryadapter.h"

namespace rhdl {

class CValue;

template <bool OWNING=true>
class ComplexCObject : public CValueContainer {
	using Super = CValueContainer;
	using PT = typename std::conditional<
			OWNING, std::unique_ptr<const CObject>, const CObject*>::type;
	using Dict = MutableDictionary<PT>;
	using DictPtr = std::unique_ptr<Dict>;

public:

	ComplexCObject(
			rhdl_type typeId, std::string name,
			DictPtr dict = std::make_unique<LexicalDictionary<PT>>())
			: Super(typeId, name), dict_(std::move(dict)), dict_deref_(*dict_)
	{
		c_.content().members = c_strings().data();
	}

	ComplexCObject(ComplexCObject &&);

	virtual ~ComplexCObject() {};

	const CObject &add(PT member)
	{
		Super::updateContainerFor(*member, *this);

		auto &ptr = dict_ -> add(std::move(member));

		if (!ptr) {
			throw ConstructionException(Errorcode::E_MEMBER_EXISTS, member -> name());
		}

		setMembers();
		return *ptr;
	}

	const CObject &replace(PT member)
	{
		Super::updateContainerFor(*member, *this);

		auto &ptr = dict_ -> replace(std::move(member));

		if (!ptr) {
			throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER, member -> name());
		}

		setMembers();
		return *ptr;
	}

	std::size_t size() const override {return dict_ -> size();}

protected:
	void clearDict() {dict_ -> clear();}

#if 0
	auto begin() {dict_ -> begin();}
	auto end() {dict_ -> end();}
#endif

private:
	const Dictionary<const CObject &> &dictionary() const override {return dict_deref_;}

	const CObject &add(const CValue &cvalue) {
		return add(static_cast<PT>(&cvalue));
	}

	virtual const CObject &add_after_move(const CValue &cvalue) {
		return replace(static_cast<PT>(&cvalue));
	}

	DictPtr dict_;
	DereferencingDictionaryAdapter<Dict> dict_deref_;
};


template<bool OWNING>
inline ComplexCObject<OWNING>::ComplexCObject(
		ComplexCObject &&moved) :
				Super(std::move(moved)),
				dict_(std::move(moved.dict_)),
				dict_deref_(*dict_)
{}

}

#endif //SRC_C_API_COMPLEXCOBJECT_H_
