/*
 * typedportset.h
 *
 *  Created on: Aug 7, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDPORTSET_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDPORTSET_H_

#include "abstractpoly.h"

namespace rhdl::polycontainer {

template <class Container, class... LINEAGE>
class PolyBase : public AbstractPoly<typename remove_unique_ptr<typename Container::value_type>::type, LINEAGE...> {
protected:
	using StoredValue = typename Container::value_type;

private:
	using Value = typename remove_unique_ptr<StoredValue>::type;
	using Super = AbstractPoly<Value, LINEAGE...>;

protected:
	using Top = typename Super::Top;

private:
	using TopPolyIter = typename AbstractPoly<Top>::PolyIterator;
	using CIterator = typename Container::iterator;

	struct IteratorBase : Super::PolyIterator {
		IteratorBase(CIterator i) : ic_(i) {};
		IteratorBase(const IteratorBase &i) = default;

		virtual ~IteratorBase() {}

	protected:
		CIterator ic_;
	};

	template <class NP>
	struct  PNPIterator : IteratorBase {
	protected:
		using IteratorBase::ic_;

	public:
		using IteratorBase::IteratorBase;

		Value &operator*() const override {return *ic_;}
		Value *operator->() const override {return &*ic_;}
	};

	template <class P>
	struct PNPIterator<std::unique_ptr<P>> : IteratorBase {
	protected:
		using IteratorBase::ic_;

	public:
		using IteratorBase::IteratorBase;

		Value &operator*() const override {return **ic_;}
		Value *operator->() const override {return ic_ -> get();}
	};

public:
	class iterator : public PNPIterator<StoredValue> {
		using Super = PNPIterator<StoredValue>;
		using Super::ic_;

	public:
		using PNPIterator<StoredValue>::PNPIterator;

		operator const CIterator &() const {return ic_;}

		iterator &operator=(const iterator &i) = default;
		iterator& operator++() override {++ic_; return *this;}
		iterator operator++(int);
		bool operator==(const iterator &i) const {return ic_ == i.ic_;}
		bool operator!=(const iterator &i) const {return ic_ != i.ic_;}

	private:
		iterator *clone() const override {return new iterator(ic_);}
	};

private:
	template <class C>
	struct GetLess {
		struct type {};
	};

	template <template <class...> class C, class Compare, class... Stuff>
	struct GetLess<C<Value, Compare, Stuff...>> {
		using type = Compare;
	};

	template <template <class...> class C, class Compare, class... Stuff>
	struct GetLess<C<std::unique_ptr<Value>, Compare, Stuff...>> {
		struct type {
			constexpr bool operator()(
					const std::unique_ptr<Value> &lhs,
					const std::unique_ptr<Value> &rhs)
			{
				return (*this)(*lhs, *rhs);
			}

			constexpr bool operator()(
					const std::unique_ptr<Value> &lhs, const Value &rhs)
			{
				return (*this)(*lhs, rhs);
			}

			constexpr bool operator()(
					const Value &lhs, const std::unique_ptr<Value> &rhs)
			{
				return (*this)(lhs, *rhs);
			}
		};
	};

public:
	template <class... Args>
	PolyBase(Args&&... args) : container_(std::forward<Args...>(args)...) {}

	PolyBase(const PolyBase &pb) : container_(pb.container_) {}
	PolyBase(PolyBase &&pb) : container_(std::move(pb.container_)) {}
	//PolyBase(PolyBase pb) : PolyBase(std::move(pb)) {}

	virtual ~PolyBase() {}

	iterator begin() const {return iterator(container_.begin());}
	iterator end() const {return iterator(container_.end());}
	template <class K> iterator find(const K &p) const {return iterator(container_.find(p));}
	iterator erase(const iterator &i) {return iterator(container_.erase(i));}
	size_t erase(const StoredValue &v) {return container_.erase(v);}
	size_t erase(const Top &v);
	bool empty() const override {return container_.empty();}
	size_t size() const override {return container_.size();}
	void clear() {container_.clear();}
	typename Container::node_type extract(const Value &p);
	typename Container::node_type extract(iterator i) {return container_.extract(i);}

private:
	iterator *pbegin() const override {return new iterator(container_.begin());}
	iterator *pend() const override {return new iterator(container_.end());}
	iterator *pfind(const Top &v) const override {return new iterator(container_.find(v));}
	iterator *perase(const TopPolyIter &i) override;
	size_t perase(const Top &v) override {return erase(v);}

protected:
	Container container_;
};


template<class Container, class ... LINEAGE>
size_t PolyBase<Container, LINEAGE...>::erase(const Top &v)
{
	auto i = container_.template find<Top>(v);

	if (i == container_.end())
		return 0;

	container_.erase(i);
	return 1;
}

template<class Container, class... LINEAGE>
inline typename PolyBase<Container, LINEAGE...>::iterator *
PolyBase<Container, LINEAGE...>::perase(const TopPolyIter &i)
{
	auto fit = dynamic_cast<const iterator *>(&i);
	assert (fit);
	return new iterator(container_.erase(*fit));
}

template<class Container, class... LINEAGE>
inline typename PolyBase<Container, LINEAGE...>::iterator
PolyBase<Container, LINEAGE...>::iterator::operator ++(int)
{
	auto isetcpy(ic_);
	++ic_;
	return iterator(isetcpy);
}

} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDPORTSET_H_ */
