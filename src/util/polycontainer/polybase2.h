/*
 * polybase2.h
 *
 *  Created on: Sep 8, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_POLYBASE2_H_
#define SRC_UTIL_POLYCONTAINER_POLYBASE2_H_

#include "polybase.h"

#include "util/list.h"

namespace rhdl {
namespace polycontainer {
namespace detail {

template <class Container, class Lineage, class... LINEAGE>
class PolyBase2;

template <class Container, class Lineage, class Value, class... LINEAGE>
struct PolyBase2<Container, Lineage, Value, LINEAGE...> : PolyBase2<Container, Lineage, LINEAGE...> {
	using Super = PolyBase2<Container, Lineage, LINEAGE...>;

protected:
	using Super::Top;
	using iterator = typename Super::iterator;

public:
	template <class... Args>
	PolyBase2(Args&&... args) : Super(std::forward<Args...>(args)...) {}
	PolyBase2(const PolyBase2 &p) : Super(static_cast<const Super &>(p)) {}
	PolyBase2(PolyBase2 &&p) : Super(static_cast<Super &&>(p)) {}

private:
	using SPoly = AbstractPoly<Value>;
	using SPolyIterator = typename SPoly::PolyIterator;

	iterator *pfind(const Value &v) const override {return new iterator(this -> find(v));}
	size_t perase(const Value &v) override {return this -> erase(v);}
	iterator *perase(const SPolyIterator &i) override
	{
		auto fit = dynamic_cast<const typename Super::iterator *>(&i);
		assert (fit);
		return new iterator(this -> erase(*fit));
	}
};

template <class Container, class... LINEAGE>
class PolyBase2<Container, List<LINEAGE...>> : public PolyBase<Container, LINEAGE...> {
	using Super = PolyBase<Container, LINEAGE...>;

public:
	template <class... Args>
	PolyBase2(Args&&... args) : Super(std::forward<Args...>(args)...) {}
	PolyBase2(const PolyBase2 &p) : Super(static_cast<const Super &>(p)) {}
	PolyBase2(PolyBase2 &&p) : Super(static_cast<Super &&>(p)) {}
};

} /* namespace detail */
} /* namespace polycontainer */
} /* namespace rhdl */

#endif /* SRC_UTIL_POLYCONTAINER_POLYBASE2_H_ */
