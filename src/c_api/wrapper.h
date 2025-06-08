/*
 * wrapper.h
 *
 *  Created on: May 18, 2021
 *      Author: ovenror
 */

#ifndef C_API_WRAPPER_H_
#define C_API_WRAPPER_H_

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/constructionexception.h>
#include <type_traits>

namespace rhdl {

template <class C_Struct>
struct Wropper {
	C_Struct content_;
};

template <class CPP_Class>
class Wrapper : public Wropper<typename CPP_Class::C_Struct> {
public:
	using C_Struct = typename CPP_Class::C_Struct;
	using Super = Wropper<C_Struct>;

	Wrapper(CPP_Class &cpp) : cpp_(&cpp) {}
	Wrapper(Wrapper &&moved, CPP_Class &cpp);
	~Wrapper() {magic_ = 0;}

	C_Struct &content() {return Super::content_;}
	const C_Struct &content() const {return Super::content_;}

	operator const C_Struct &() const {return content();}

	static CPP_Class &recover(const C_Struct *c)
	{
		auto *wrop = reinterpret_cast<const Super *>(c);
		auto *wrapper = static_cast<const Wrapper *>(wrop);
		return const_cast<Wrapper *>(wrapper) -> recover();
	}

	static C_Struct *c_ptr(CPP_Class &cpp)
	{
		return &cpp.c_.content();
	}

	static const C_Struct *c_ptr(const CPP_Class &cpp)
	{
		return &cpp.c_.content();
	}

	static C_Struct *c_ptr(CPP_Class *cpp)
	{
		return cpp ? &cpp -> c_.content() : nullptr;
	}

	static const C_Struct *c_ptr(const CPP_Class *cpp)
	{
		return cpp ? &cpp -> c_.content() : nullptr;
	}

	void updateCPP(CPP_Class *cpp) {
		cpp_ = cpp;
	}

private:
	CPP_Class &recover() {
		if (magic_ != Magic)
			throw ConstructionException(Errorcode::E_UNKNOWN_STRUCT);

		if (id_ != CPP_Class::C_ID)
			throw ConstructionException(Errorcode::E_WRONG_STRUCT_TYPE);

		return *cpp_;
	}

	static constexpr long int Magic = 0x04D100A61C000B30;
	long int magic_ = Magic;
	const long int id_ = CPP_Class::C_ID;
	CPP_Class *cpp_;
};

template <class CPP_Class>
CPP_Class &recover(const typename CPP_Class::C_Struct *c)
{
	return Wrapper<CPP_Class>::recover(c);
}

template <class CPP_Class>
const typename CPP_Class::C_Struct *c_ptr(const CPP_Class &cpp)
{
	return Wrapper<CPP_Class>::c_ptr(cpp);
}

template <class CPP_Class>
typename CPP_Class::C_Struct *c_ptr(CPP_Class &cpp)
{
	return Wrapper<CPP_Class>::c_ptr(cpp);
}

template <class CPP_Class>
const typename CPP_Class::C_Struct *c_ptr(const CPP_Class *cpp)
{
	return Wrapper<CPP_Class>::c_ptr(cpp);
}

template <class CPP_Class>
typename CPP_Class::C_Struct *c_ptr(CPP_Class *cpp)
{
	return Wrapper<CPP_Class>::c_ptr(cpp);
}

template<class CPP_Class>
Wrapper<CPP_Class>::Wrapper(Wrapper &&moved, CPP_Class &cpp)
	: cpp_(&cpp)
{
	cpp.c_.content_ = moved.content_;
}

}

#endif /* C_API_WRAPPER_H_ */
