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
#include "c_api/wrapper.h"

#include <cassert>
#include <memory>

namespace rhdl {

class Entity;
class Namespace;

class CObject : public dictionary::Dictionary<const CObject>
{
	template <class T>
	using Dictionary = dictionary::Dictionary<T>;

public:
	CObject(rhdl_type typeId, std::string name);
	CObject(CObject &&moved);

	virtual ~CObject();

	const std::string &name() const {return name_;}
	const std::string fqn() const;
	const CObject *container() const {return container_;}

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	const CObject& at(const char *name) const override;
	const CObject& at(const std::string &name) const override;

	size_t size() const override;

	operator const rhdl_object *const() const {return c_ptr(*this);}

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
	template <class DICT> const typename DICT::StoreType &add(
			DICT &d, typename DICT::StoreType member);
	template <class DICT> const typename DICT::StoreType &replace(
			DICT &d, typename DICT::StoreType member);

	void setDictionaryPtr(std::unique_ptr<Dictionary<const CObject>> dict);
	template <class DICT> void setDictionary(DICT d);
	const CStrings &c_strings() const override;
	virtual void setMembers();

private:
	template <class T> void updateContainerFor(T &member);
	template <class T> void updateContainerFor(T* member);
	template <class T> void updateContainerFor(std::unique_ptr<T> &member);

	void assertInitialized() const;
	void updateContainer(const CObject &c) const {container_ = &c;}

	const std::string name_;
	std::unique_ptr<Dictionary<const CObject>> dict_;
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

template<class DICT>
inline const typename DICT::StoreType &CObject::add(DICT &d, typename DICT::StoreType member)
{
	if (dict_)
		assert(d.is_same_as(*dict_));

	updateContainerFor(member);
	const typename DICT::StoreType *result ;

	try {
		result = &d.add(std::move(member));
	} catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_MEMBER_EXISTS, member -> name());
	}

	if (dict_)
		setMembers();

	return *result;
}

template<class DICT>
inline const typename DICT::StoreType &CObject::replace(DICT &d, typename DICT::StoreType member)
{
	if (dict_)
		assert(d.is_same_as(*dict_));

	updateContainerFor(member);
	const typename DICT::StoreType *result ;

	try {
		result = &d.replace(std::move(member));
	} catch (std::out_of_range &e) {
		throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER, member -> name());
	}

	if (dict_)
		setMembers();

	return *result;
}

template<class DICT>
inline void CObject::setDictionary(DICT d)
{
	setDictionaryPtr(std::make_unique<DICT>(std::move(d)));
}


}

#endif /* SRC_C_API_COBJECT_H_ */
