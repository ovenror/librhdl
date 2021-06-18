/*
 * portset.h
 *
 *  Created on: Jul 29, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSET_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSET_H_

#include "config.h"

//#include "polyiterator.h"
//#include "polyiteratoradapter.h"
//#include "virtuals.h"

#include <memory>
#include <set>

namespace rhdl::polycontainer::detail {

template <class Config>
class AbstractPolyImpl;

template <class Value, class... BASES>
class AbstractPolyImpl<Config<Value, BASES...>> : public BASES...
{
protected:
	class PolyIterator;

private:
	template <class... BASES2>
	struct GetTop {
		using TopPolyIterator = PolyIterator;
		using Top = Value;
	};

	template <class Single, class Lineage>
	struct GetTop<Single, Lineage> {
		using TopPolyIterator = typename Lineage::PolyIterator;
		using Top = typename Lineage::Top;
	};

	using ThisGetTop = GetTop<BASES...>;

protected:
	using This = AbstractPolyImpl<Config<Value, BASES...>>;
	using Top = typename ThisGetTop::Top;
	using TopPolyIterator = typename ThisGetTop::TopPolyIterator;

	struct PolyIterator : BASES::PolyIterator... {
		using value_type = Value;

		bool operator==(const PolyIterator &i) const {return &**this == &*i;}
		bool operator!=(const PolyIterator &i) const {return !(*this == i);}

		virtual PolyIterator& operator++() = 0;
		virtual Value *operator->() const = 0;
		virtual Value &operator*() const = 0;

		virtual ~PolyIterator() {}

		virtual PolyIterator *clone() const = 0;
	};

public:
	struct iterator : BASES::IteratorConversions... {
		using value_type = Value;

		iterator(PolyIterator *poly) : poly_(poly) {}

		iterator(std::unique_ptr<PolyIterator> &&poly)
			: poly_(std::move(poly)) {}

		iterator(const iterator &i) : poly_(i.poly_ -> clone()) {}
		iterator(iterator &&i) : iterator(std::move(i.poly_)) {}

		operator PolyIterator &() {return *poly_;}

		iterator &operator=(const iterator &i) {poly_ = unique_ptr(i.poly_.clone());}
		iterator &operator=(iterator &&i) {return poly_ = std::move(i.poly_);}

		iterator& operator++() {++*poly_; return *this;}
		iterator operator++(int);

		auto *operator->() const {return poly_ -> operator->();}
		auto &operator*() const {return **poly_;}

		bool operator==(const iterator &i) const {return *poly_ == *i.poly_;}
		bool operator!=(const iterator &i) const {return *poly_ != *i.poly_;}

	private:
		const PolyIterator &poly() const {return *poly_;}

		std::unique_ptr<PolyIterator> poly_;
	};

public:
	virtual ~AbstractPolyImpl() {}

	iterator begin() const {return iterator(pbegin());}
	iterator end() const {return iterator(pend());}
	iterator find(const Value &v) {return iterator(pfind(v));}
	iterator erase(const iterator &i) {return iterator(perase(i));}
	size_t erase(const Value &v) {return iterator(perase(v));}

	virtual bool empty() const = 0;
	virtual size_t size() const = 0;

protected:
	struct IteratorConversions : BASES::IteratorConversions... {
		operator iterator() {return iterator(poly());}
		virtual const PolyIterator &poly() const = 0;
	};

private:
	virtual PolyIterator *pbegin() const = 0;
	virtual PolyIterator *pend() const = 0;

	virtual PolyIterator *pfind(const Top &) const = 0;
	virtual PolyIterator *perase(const TopPolyIterator &) = 0;
	virtual size_t perase(const Top &) = 0;
};

template <class Value, class... BASES>
inline typename AbstractPolyImpl<Config<Value, BASES...>>::iterator
AbstractPolyImpl<Config<Value, BASES...>>::iterator::operator ++(int)
{
	iterator cpy(*this);
	poly_++;
	return cpy;
}

} /* namespace rhdl::polycontainer::detail */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSET_H_ */
