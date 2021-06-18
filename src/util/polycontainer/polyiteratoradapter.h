/*
 * polyiteratoradapter.h
 *
 *  Created on: Aug 22, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_POLYITERATORADAPTER_H_
#define SRC_UTIL_POLYCONTAINER_POLYITERATORADAPTER_H_

#include <memory>

namespace rhdl::polycontainer::detail {

template <class PI>
struct PolyIteratorAdapter {
	using value_type = typename PI::value_type;

	PolyIteratorAdapter(const PI &poly)	{poly_(unique_ptr(poly.clone()));}
	PolyIteratorAdapter(PI &&poly) {poly_(std::make_unique(poly));}

	PolyIteratorAdapter(std::unique_ptr<PI> &&poly)
		: poly_(std::move(poly)) {}

	PolyIteratorAdapter(const PolyIteratorAdapter &i) : PolyIteratorAdapter(i.poly_) {}
	PolyIteratorAdapter(PolyIteratorAdapter &&i) : PolyIteratorAdapter(std::move(i.poly_)) {}

	PolyIteratorAdapter &operator=(const PolyIteratorAdapter &i) {poly_ = unique_ptr(i.poly_.clone());}
	PolyIteratorAdapter &operator=(PolyIteratorAdapter &&i) {return poly_ = std::move(i.poly_);}

	PolyIteratorAdapter& operator++() {++*poly_; return *this;}
	PolyIteratorAdapter operator++(int);

	auto *operator->() const {return poly_ -> operator->();}
	auto &operator*() const {return **poly_;}

	bool operator==(const PolyIteratorAdapter &i) const {return *poly_ == *i.poly_;}
	bool operator!=(const PolyIteratorAdapter &i) const {return *poly_ != *i.poly_;}

private:
	const PI &poly() const {return *poly_;}

	std::unique_ptr<PI> poly_;
};

template<class PI>
inline PolyIteratorAdapter PolyIteratorAdapter<PI>::operator ++(int)
{
	PolyIteratorAdapter cpy(*this);
	poly_++;
	return cpy;
}


} /* namespace rhdl::polycontainer::detail */

#endif /* SRC_UTIL_POLYCONTAINER_POLYITERATORADAPTER_H_ */
