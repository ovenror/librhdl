/*
 * partitionclass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_PARTITIONCLASSBASE_H_
#define SRC_UTIL_PARTITIONCLASSBASE_H_

#include "dereference_iterator.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <type_traits>
#include <set>

namespace rhdl {

template <class Element>
class PartitionClassBase {
public:
	using Owner = typename Element::Owner;
	using PartitionClassRoot = PartitionClassBase;
	using Container = std::set<Element *, typename Element::LessComparator>;
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

	Element &accept(Element &);
	Element &accept(Element *e) {return accept(*e);}

	Element &acceptClassed(Element &);
	Element &acceptClassed(Element *e) {return acceptClassed(*e);}

	Element &acceptStray(Element &);
	Element &acceptStray(Element *e) {return acceptStray(*e);}

	void ingest(PartitionClassBase &victim);

	void kill(Element &element) {reject(element);}
	void kill(iterator element) {reject(element);}

	Element &reject(Element &element);
	Element &reject(iterator element);

	void clear();

	Owner &owner() {return owner_;}

	PartitionClassBase(Owner &o);
	PartitionClassBase(Element &first, Owner &o);
	PartitionClassBase(Element *first, Owner &o) : PartitionClassBase(*first, o) {}
	PartitionClassBase(PartitionClassBase &&victim, Owner &o);
	PartitionClassBase(PartitionClassBase &&victim);
	PartitionClassBase(const PartitionClassBase &victim) = delete;

	PartitionClassBase &operator=(const PartitionClassBase &) = delete;
	PartitionClassBase &operator=(PartitionClassBase &&);

protected:
	void clear_norelease();

private:
	void onAssign(Element &) {}
	void onReassign(Element &) {}

	void kill(ptrIterator element) {reject(element);}
	Element &reject(ptrIterator element);
	Element &reject_norelease(ptrIterator element);

	void assign(Element &element);
	void reassign(Element &element);
	void assign_internal(Element &element);

	void reassign_all(const PartitionClassBase &victim);

	Owner &owner_;
	Container elements_;
};

template<class Element>
inline PartitionClassBase<Element>::PartitionClassBase(Owner &o)
	: owner_(o)
{}

template <class Element>
inline PartitionClassBase<Element>::PartitionClassBase(
		Element &element, Owner &o)
	: owner_(o)
{
	elements_.insert(&element);
	assign(element);
}

template<class Element>
inline void PartitionClassBase<Element>::reassign_all(
		const PartitionClassBase &victim)
{
	for (Element &e : *this) {
		assert (&e.pclass_ == &victim);
		reassign(e);
	}
}

template<class Element>
inline PartitionClassBase<Element> &PartitionClassBase<Element>::operator =(
		PartitionClassBase &&victim)
{
	elements_ = std::move(victim.elements_);
	reassign_all(victim);
	return *this;
}

template<class Element>
inline PartitionClassBase<Element>::PartitionClassBase(
		PartitionClassBase &&victim, Owner &o)
	: elements_(std::move(victim.elements_)), owner_(o)
{
	reassign_all(victim);
}

template<class Element>
inline PartitionClassBase<Element>::PartitionClassBase(
		PartitionClassBase &&victim)
	: PartitionClassBase<Element>(victim, victim.owner_)
{}

template<class Element>
inline void PartitionClassBase<Element>::reassign(
		Element& element)
{
	assert (element.pclass_);
	assign_internal(element);
	owner_.onReassign(element);
}

template<class Element>
inline void PartitionClassBase<Element>::assign(
		Element& element)
{
	assert (!element.pclass_);
	assign_internal(element);
	owner_.onAssign(element);
}

template<class Element>
inline Element& PartitionClassBase<Element>::accept(Element &e)
{
	if (e.pclass_)
		return acceptClassed(e);
	else
		return acceptStray(e);
}

template<class Element>
inline void PartitionClassBase<Element>::clear()
{
	if (elements_.empty())
		return;

	clear_norelease();
	Owner::release(*this, owner_);
}

template<class Element>
inline void PartitionClassBase<Element>::clear_norelease()
{
	while (!elements_.empty()) {
		reject_norelease(elements_.begin());
	}
}

template<class Element>
inline void rhdl::PartitionClassBase<Element>::assign_internal(
		Element &element)
{
	element.pclass_ = this;
}


template<class Element>
inline Element &PartitionClassBase<Element>::acceptStray(
		Element &element)
{
	assign(element);
	elements_.insert(&element);
	return element;
}

template<class Element>
inline Element &PartitionClassBase<Element>::acceptClassed(
		Element &element)
{
	assert(element.pclass_);
	ingest(*element.pclass_);
	return element;
}

template<class Element>
inline void PartitionClassBase<Element>::ingest(PartitionClassBase<Element> &victim) {
	auto &vOwner = victim.owner_;

	auto &migrating = victim.elements_;

	for (auto &migrant : migrating) {
		reassign(*migrant);
		elements_.emplace(migrant);
	}

	migrating.clear();
	vOwner.release(victim);
}

template<class Element>
inline Element &PartitionClassBase<Element>::reject(
		Element& element)
{
	assert (element.pclass_ == this);

	auto iter = elements_.find(element);
	assert (iter != elements_.end());
	return reject(iter);
}

template <class Element>
inline Element &PartitionClassBase<Element>::reject(
		iterator element)
{
	return reject(element.base());
}

template<class Element>
inline Element &PartitionClassBase<Element>::reject_norelease(
		ptrIterator element)
{
	assert ((*element) -> pclass_ == this);

	//Owner::onReject(**element);

	auto eptr = std::move(*element);

	//TODO: Not necessary for OwningPartitionClass
	eptr -> pclass_ = nullptr;

	elements_.erase(element);

	return *eptr;
}

template<class Element>
inline Element &PartitionClassBase<Element>::reject(
		ptrIterator element)
{
	auto &rejected = reject_norelease(element);

	if (empty())
		owner_.release(*this);

	return rejected;
}

template <class Element>
template <class K>
inline bool PartitionClassBase<Element>::contains(const K &k) const
{
	return elements_.find(k) != elements_.end();
}

} /* namespace rhdl */

#endif /* SRC_UTIL_PARTITIONCLASSBASE_H_ */
