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

template <class Derived, class PartitionClass, class Key = None>
class Partitionable {
	template <class> struct GetTargetKey;
	class DefaultKey;
	using KeyBase = typename std::conditional<
			std::is_same<Key, None>::value,
			DefaultKey, Key>::type;
	class CombinedKey;
	using TargetKey = typename GetTargetKey<KeyBase>::type;
public:
	class Less;

	Partitionable() {}
	virtual ~Partitionable() {}

	operator TargetKey() const;
	PartitionClass *pclass() const {return pcOwner_;}

private:
	template <class> friend class PartitionClassBase;
	template <class> friend class PointingPartitionClass;
	using Owner = PartitionClass;

	//void releasePartitionClass() {PartitionClass::release(*pcOwner_);}

	PartitionClass *pcOwner_ = nullptr;
};

template<class Derived, class PartitionClass, class Key>
template <class CombinedKey>
struct Partitionable<Derived, PartitionClass, Key>::GetTargetKey {
	using type = decltype(std::declval<CombinedKey>().operator()(std::declval<const Derived &>()));
};

template<class Derived, class PartitionClass, class Key>
struct Partitionable<Derived, PartitionClass, Key>::DefaultKey
{
	constexpr Derived *operator()(const Derived &element) const noexcept
	{
		return &element;
	}
};

template<class Derived, class PartitionClass, class Key>
class Partitionable<Derived, PartitionClass, Key>::CombinedKey : public KeyBase {
protected:
	using KeyBase::operator();

private:
	TargetKey operator()(const Derived *element) const noexcept
	{
		return operator()(*element);
	}

	TargetKey operator()(const std::unique_ptr<Derived> &element) const noexcept
	{
		return operator()(static_cast<const Derived &>(*element));
	}

	TargetKey operator()(const TargetKey &key) const noexcept
	{
		return key;
	}

#if 0
	TargetKey operator()(const Derived &element) const noexcept
	{
		return KeyBase::operator()(element);
	}
#endif

public:
	template <class K>
	TargetKey key(const K &convertible_key) const
	{
		return operator()(convertible_key);
	}
};

template<class Derived, class PartitionClass, class Key>
class Partitionable<Derived, PartitionClass, Key>::Less : public CombinedKey
{
public:
	using is_transparent = void;
	using CombinedKey::key;

	bool operator()(const Derived &lhs, const Derived *rhs) const
	{
		return key(lhs) < key(rhs);
	}

	bool operator()(const TargetKey &lhs, const Derived *rhs) const
	{
		return lhs < key(rhs);
	}

	bool operator()(const TargetKey &lhs, const std::unique_ptr<Derived> &rhs) const
	{
		return lhs < key(rhs);
	}

	bool operator()(const Derived *lhs, const Derived &rhs) const
	{
		return key(lhs) < key(rhs);
	}

	bool operator()(const Derived *lhs, const TargetKey &rhs) const
	{
		return key(lhs) < rhs;
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
