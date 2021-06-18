/*
 * poly.h
 *
 *  Created on: Aug 14, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLY_H_
#define SRC_UTIL_POLY_H_

#include <vector>

namespace rhdl::polycontainer {

template <class Container, class Super>
class PolyImpl : public Super {
	using typename Super::iterator;
	using Super::container_;

public:
	template <class... Args>
	PolyImpl(Args&&... args) : Super(std::forward<Args...>(args)...) {}
	PolyImpl(const PolyImpl &p) : Super(static_cast<const Super &>(p)) {}
	PolyImpl(PolyImpl &&p) : Super(static_cast<Super &&>(p)) {}

	std::pair<iterator, bool> insert(typename Super::StoredValue);

	template <class... Args>
	std::pair<iterator, bool> emplace(Args&&... args);
};

template <class Container, class Super>
inline std::pair<typename PolyImpl<Container, Super>::iterator,bool>
PolyImpl<Container, Super>::insert(typename Super::StoredValue v)
{
	auto [it, ok] = container_.insert(std::move(v));
	return {iterator(it), ok};
}

template <class Container, class Super>
template <class... Args>
inline std::pair<typename PolyImpl<Container, Super>::iterator,bool>
PolyImpl<Container, Super>::emplace(Args&&... args)
{
	auto [it, ok] = container_.emplace(std::forward<Args...>(args)...);
	return {iterator(it), ok};
}

template <class... Stuff, class Super>
class PolyImpl<std::vector<Stuff...>, Super> : public Super {
	using Super::container_;

public:
	template <class... Args>
	PolyImpl(Args&&... args) : Super(std::forward(args)...) {}
	PolyImpl(const PolyImpl &p) : Super(static_cast<const Super &>(p)) {}
	PolyImpl(PolyImpl &&p) : Super(static_cast<Super &&>(p)) {}

	void push_back(typename Super::StoredValue);

	template <class... Args>
	void emplace_back(Args&&...);
};

template<class... Stuff, class Super>
inline void PolyImpl<std::vector<Stuff...>, Super>::push_back(typename Super::StoredValue v)
{
	container_.push_back(std::move(v));
}

template <class... Stuff, class Super>
template<class... Args>
inline void PolyImpl<std::vector<Stuff...>, Super>::emplace_back(Args&&... args)
{
	container_.emplace_back(std::forward(args)...);
}


} /* namespace rhdl */

#endif /* SRC_UTIL_POLY_H_ */
