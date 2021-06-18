/*
 * partitionclass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_PARTITIONCLASSBASE_H_
#define SRC_UTIL_PARTITIONCLASSBASE_H_

#include <algorithm>
#include <cassert>
#include <memory>
#include <type_traits>
#include <set>

#include "util/dereference_iterator.h"

namespace rhdl {

template <class Element>
class PartitionClassBase {
public:
	using PartitionClassRoot = PartitionClassBase;
	using Container = std::set<Element *, typename Element::Less>;
	using ptrIterator = typename Container::iterator;
	using iterator = DereferencingIterator<ptrIterator>;
	using const_iterator = ConstDereferencingIterator<ptrIterator>;
	using size_type = typename Container::size_type;

	virtual ~PartitionClassBase() {}

	const_iterator cbegin() const {return const_deref_iterator(elements_.begin());}
	const_iterator cend() const {return const_deref_iterator(elements_.end());}
	iterator begin() const {return deref_iterator(elements_.begin());}
	iterator end() const {return deref_iterator(elements_.end());}
	iterator begin() {return deref_iterator(elements_.begin());}
	iterator end() {return deref_iterator(elements_.end());}

	template <class K>
	const_iterator find(const K &k) const {
		return const_deref_iterator(elements_.find(elements_.key_comp().key(k)));
	}
	template <class K>
	iterator find(const K &k) {
		return deref_iterator(elements_.find(elements_.key_comp().key(k)));
	}
	template <class K>
	bool contains(const K &k) const;

	bool empty() const {return elements_.empty();}

	void accept(Element &element);
	void acceptStray(Element &element);
	void ingest(PartitionClassBase &victim);
	static void kill(Element &element) {reject(element);}
	static Element &reject(Element &element);

protected:
	using Owner = typename Element::Owner;

protected:
	PartitionClassBase(Element &element, Owner &o);
	PartitionClassBase(PartitionClassBase &&victim, Owner &o);

private:
	void kill(ptrIterator element) {reject(element);}
	Element &reject(ptrIterator element);

	void assign(Element &element);
	void reassign(Element &element);
	void assign_internal(Element &element);

	Owner *&owner_ptr();
	Owner &owner() {return *owner_ptr();}

	Container elements_;
};

template <class Element>
inline PartitionClassBase<Element>::PartitionClassBase(
		Element &element, Owner &o)
{
	elements_.insert(&element);
	element.pcOwner_ = &o;
}

template<class Element>
inline PartitionClassBase<Element>::PartitionClassBase(
		PartitionClassBase &&victim, Owner &o)
	: elements_(std::move(victim.elements_))
{
	for (Element &e : *this) {
		assert (e.pcOwner_ == &victim.owner());
		e.pcOwner_ = &o;
	}
}


template<class Element>
inline void PartitionClassBase<Element>::reassign(
		Element& element)
{
	assert (element.pcOwner_);
	assign_internal(element);
	owner().onReassign(element);
}

template<class Element>
inline void PartitionClassBase<Element>::assign(
		Element& element)
{
	assert (!element.pcOwner_);
	assign_internal(element);
	owner().onAssign(element);
}

template<class Element>
inline void rhdl::PartitionClassBase<Element>::assign_internal(
		Element &element)
{
	element.pcOwner_ = owner_ptr();
}


template<class Element>
inline void PartitionClassBase<Element>::acceptStray(
		Element &element)
{
	assign(element);
	elements_.insert(&element);
}

template<class Element>
inline void PartitionClassBase<Element>::accept(
		Element &element)
{
	auto owner = element.pcOwner_;
	assert (owner);
	ingest(owner -> pclass());
}

template<class Element>
inline void PartitionClassBase<Element>::ingest(PartitionClassBase<Element> &victim) {
	auto &vOwner = victim.owner();
	owner().onIngest(vOwner);

	auto &migrating = victim.elements_;

	for (auto &migrant : migrating) {
		reassign(*migrant);
		elements_.emplace(migrant);
	}

	Owner::release(vOwner);
}

template<class Element>
inline Element &PartitionClassBase<Element>::reject(
		Element& element)
{
	auto &owner = *element.pcOwner_;
	auto &pc = owner.pclass();
	auto iter = pc.elements_.find(pc.elements_.key_comp().key(element));
	assert (iter != pc.elements_.end());
	return pc.reject(iter);
}

template<class Element>
inline Element &PartitionClassBase<Element>::reject(
		ptrIterator element)
{
	//Owner::onReject(**element);

	auto eptr = std::move(*element);
	auto &owner = *eptr -> pcOwner_;
	eptr -> pcOwner_ = nullptr;

	elements_.erase(element);

	if (empty())
		Owner::release(owner);

	return *eptr;
}

template <class Element>
template <class K>
inline bool PartitionClassBase<Element>::contains(const K &k) const
{
	return elements_.find(k) != elements_.end();
}

template<class Element>
inline typename rhdl::PartitionClassBase<Element>::Owner *&rhdl::PartitionClassBase<
		Element>::owner_ptr()
{
	auto first = elements_.begin();
	assert (first != elements_.end());
	return (*first) -> pcOwner_;
}

} /* namespace rhdl */

#endif /* SRC_UTIL_PARTITIONCLASSBASE_H_ */
