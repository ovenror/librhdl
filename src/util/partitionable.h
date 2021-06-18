/*
 * partitionable.h
 *
 *  Created on: Jun 16, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_PARTITIONABLE_H_
#define SRC_UTIL_PARTITIONABLE_H_

#include "owningpartitionclass.h"
#include "pointingpartitionclass.h"

#include <memory>
#include <type_traits>

namespace rhdl {

template <class> class PartitionClassBase;

template <class Derived, class OWNER, class Key = None>
class Partitionable {
	template <class> friend class PartitionClassBase;
	template <class, class> class KeyBase;
	class CombinedKey;
	using Owner = OWNER;
	class LessComparator;

public:
	Partitionable() {}
	virtual ~Partitionable() {}

	OWNER *pcOwner() const {return pclass_ ? &pclass_ -> owner() : nullptr;}
	PartitionClassBase<Partitionable> *pclass() {return pclass_;}

private:
	PartitionClassBase<Derived> *pclass_ = nullptr;
};

#if 0
template<class Derived, class OWNER, class Key>
template <class KeyBase>
struct Partitionable<Derived, OWNER, Key>::GetTargetKey {
	using type = decltype(std::declval<KeyBase>().operator()(std::declval<const Derived &>()));
};
#endif

template<class Derived, class OWNER, class Key>
template<class Key2, class Dummy>
struct Partitionable<Derived, OWNER, Key>::KeyBase : protected Key2
{
protected:
	using TargetKey = decltype(std::declval<Key2>().operator()(std::declval<const Derived &>()));
	using Key2::operator();

public:
	bool operator()(const TargetKey &lhs, const Derived *rhs) const
	{
		return lhs < (*this)(*rhs);
	}

	bool operator()(const Derived *lhs, const TargetKey &rhs) const
	{
		return (*this)(*lhs) < rhs;
	}
};

template<class Derived, class OWNER, class Key>
template<class Dummy>
struct Partitionable<Derived, OWNER, Key>::KeyBase<None, Dummy>
{
protected:
	using TargetKey = const Derived *;

	constexpr const Derived *operator()(const Derived &element) const noexcept
	{
		return &element;
	}
};

template<class Derived, class OWNER, class Key>
class Partitionable<Derived, OWNER, Key>::CombinedKey : public KeyBase<Key, None> {
	using Super = KeyBase<Key, None>;
protected:
	using Super::operator();
	using TargetKey = typename Super::TargetKey;

public:
	template <class K>
	TargetKey key(const K &convertible_key) const
	{
		return operator()(convertible_key);
	}

	TargetKey operator()(const Derived *element) const noexcept
	{
		return operator()(*element);
	}

	TargetKey operator()(const std::unique_ptr<Derived> &element) const noexcept
	{
		return operator()(static_cast<const Derived &>(*element));
	}

};

template<class Derived, class OWNER, class Key>
class Partitionable<Derived, OWNER, Key>::LessComparator : public CombinedKey
{
protected:
	using TargetKey = typename CombinedKey::TargetKey;

public:
	using is_transparent = void;
	using CombinedKey::operator();
	using CombinedKey::key;

	bool operator()(const Derived &lhs, const Derived *rhs) const
	{
		return key(lhs) < key(rhs);
	}

	bool operator()(const TargetKey &lhs, const std::unique_ptr<Derived> &rhs) const
	{
		return lhs < key(rhs);
	}

	bool operator()(const Derived *lhs, const Derived &rhs) const
	{
		return key(lhs) < key(rhs);
	}

	bool operator()(const std::unique_ptr<Derived> &lhs, const TargetKey &rhs) const
	{
		return key(lhs) < rhs;
	}

	bool operator()(const Derived *lhs, const Derived *rhs) const
	{
		return key(lhs) < key(rhs);
	}

	bool operator()(const std::unique_ptr<Derived> &lhs, const std::unique_ptr<Derived> &rhs) const
	{
		return key(lhs) < key(rhs);
	}
};

} /* namespace rhdl */

#endif /* SRC_UTIL_PARTITIONABLE_H_ */
