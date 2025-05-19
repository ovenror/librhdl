/*
 * object.h
 *
 *  Created on: Sep 16, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COBJECT_H_
#define SRC_C_API_COBJECT_H_

#include <rhdl/construction/c/types.h>
#include <util/dictionary/dictionary.h>
#include "util/cached.h"
#include "util/any_pointer.h"
#include "c_api/wrapper.h"

#include <cassert>
#include <memory>

namespace rhdl {

class Entity;
class Namespace;

class CObject
{
	using Dictionary = dictionary::Dictionary<const CObject>;

public:
	using CStrings = typename Dictionary::CStrings;

	CObject(rhdl_type typeId, std::string name);
	CObject(CObject &&moved);

	virtual ~CObject();

	const std::string &name() const {return name_;}
	const std::string fqn() const;
	const CObject *container() const {return container_;}

	bool contains(const std::string &name) const;
	bool contains(const char *name) const;

	const CObject& at(const char *name) const;
	const CObject& at(const std::string &name) const;

	size_t size() const;

	bool empty() {return !size();}

	virtual explicit operator int64_t() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator uint64_t() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator rhdl_direction() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator rhdl_reptype() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator const rhdl_iface_struct *const() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator const std::string&() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator const CObject *() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual const CObject &getRef() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual operator Entity &() {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual operator const Entity &() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual operator Namespace &() {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual operator const Namespace &() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	explicit operator const char*() const;

	virtual bool isValue() const {return false;}

protected:
	template <class DICT> const typename DICT::ValueType &add(
			DICT &d, typename DICT::ValueType &&member);
	template <class DICT> const typename DICT::ValueType &replace(
			DICT &d, typename DICT::ValueType &&member);

	void setDictionaryPtr(std::unique_ptr<Dictionary> dict);
	template <class DICT> void setDictionary(DICT &&d);
	const CStrings &c_strings() const;
	virtual void setMembers();

private:
	template <class T> void updateContainerFor(T &member);
	template <class T> void updateContainerFor(T* member);
	template <class T> void updateContainerFor(std::unique_ptr<T> &member);
	template <class T> void updateContainerFor(const std::unique_ptr<T> &member);

	void assertInitialized() const;
	void updateContainer(const CObject &c) const {container_ = &c;}

	const std::string name_;
	std::unique_ptr<Dictionary> dict_;
	mutable const CObject *container_ = nullptr;

public:
	using C_Struct = rhdl_object;

private:
	friend class Wrapper<CObject>;

	static constexpr unsigned long C_ID = 0x0813C7;

	Wrapper<CObject> c_;
};

template<class T>
inline void CObject::updateContainerFor(T &member)
{
	member.updateContainer(*this);
}

template<class T>
inline void CObject::updateContainerFor(T *member)
{
	member -> updateContainer(*this);
}

template<class T>
inline void CObject::updateContainerFor(std::unique_ptr<T> &member)
{
	member -> updateContainer(*this);
}

template<class T>
inline void CObject::updateContainerFor(const std::unique_ptr<T> &member)
{
	member -> updateContainer(*this);
}

template<class DICT>
inline const typename DICT::ValueType &CObject::add(DICT &d, typename DICT::ValueType &&member)
{
	if (dict_)
		assert(d.is_same_as(*dict_));

	updateContainerFor(member);

	if (d.contains(member -> name()))
		throw ConstructionException(Errorcode::E_MEMBER_EXISTS, member -> name());

	auto &result = d.add(std::move(member));

	if (dict_)
		setMembers();

	return result;
}

template<class DICT>
inline const typename DICT::ValueType &CObject::replace(DICT &d, typename DICT::ValueType &&member)
{
	if (dict_)
		assert(d.is_same_as(*dict_));

	updateContainerFor(member);

	if (!d.contains(member -> name()))
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER, member -> name());

	auto &result = d.replace(std::move(member));

	if (dict_)
		setMembers();

	return result;
}

namespace structural::builder { class Port; }

template<class DICT>
inline void CObject::setDictionary(DICT &&d)
{
	static_assert(!std::is_same_v<typename DICT::ValueType, structural::builder::Port>);
	static_assert(!is_any_pointer_v<typename DICT::ValueType>);
	setDictionaryPtr(static_cast<std::unique_ptr<Dictionary>>(
			std::make_unique<DICT>(std::move(d))));
}


}

#endif /* SRC_C_API_COBJECT_H_ */
