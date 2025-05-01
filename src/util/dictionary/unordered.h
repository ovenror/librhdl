/*
 * defaultorder.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_UNORDERED_H_
#define SRC_UTIL_DICTIONARY_UNORDERED_H_

#include <unordered_set>

namespace rhdl::dictionary::order {

class Unordered {
	using IsLookup = void;
	template <class> class Hash;
	template <class> class Equal;

public:
	template <class Element>
	using Container = std::unordered_set<Element, Hash<Element>, Equal<Element>>;
};

template <class Element>
class Unordered::Hash {
	using is_transparent = int;

public:
	std::size_t operator()(const Element &element) const
	{
		return namehash(element.name());
	}

	std::size_t operator()(const std::string &name) const
	{
		return namehash(name);
	}

	std::size_t operator()(const char *name) const
	{
		return namehash(name);
	}

private:
	std::hash<std::string> namehash;
};

template <class Element>
class Unordered::Equal {
	using is_transparent = int;

public:
#if 1
	bool operator()(const Element &lhs, const Element &rhs) const
	{
		return lhs == rhs;
	}

	bool operator()(const Element &lhs, const std::string &rhs) const
	{
		return lhs == rhs;
	}

	bool operator()(const std::string &lhs, const Element &rhs) const
	{
		return lhs == rhs;
	}

	bool operator()(const Element &lhs, const char *rhs) const
	{
		return lhs == rhs;
	}

	bool operator()(const char *lhs, const Element &rhs) const
	{
		return lhs == rhs;
	}
#endif

#if 0
	template <class STRING>
	bool operator()(const Element &lhs, const STRING rhs) const
	{
		return lhs == rhs;
	}

	template <class STRING>
	bool operator()(const STRING lhs, const Element &rhs) const
	{
		return lhs == rhs;
	}
#endif

#if 0
	template <class LHS, class RHS>
	bool operator()(const LHS &lhs, const RHS &rhs) const
	{
		return lhs == rhs;
	}
#endif
};

}

#endif /* SRC_UTIL_DICTIONARY_UNORDERED_H_ */
