#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include "sisingle.h"
#include "sicomposite.h"
#include "util/static_strings.h"

namespace rhdl::behavioral {

class NoSuchInterface;

template <class IF, class search_string>
struct SIGet;

template <class search_string, class... components>
struct SIGet<SimInterfaceContainer<components...>, search_string> {
	using Parent = SimInterfaceContainer<components...>;
	using Result = NoSuchInterface;

	static constexpr const Result &get(Parent &p)
	{
		return NoSuchInterface();
	}
};

template <class component0, class search_string, class... components>
struct SIGet<SimInterfaceContainer<component0, components...>, search_string> {
	using Parent = SimInterfaceContainer<component0, components...>;
	using Next = SIGet<SimInterfaceContainer<components...>, search_string>;
	using Result = typename Next::Result;

	static constexpr Result &get(Parent &p)
	{
		return Next::get(p.rest_);
	}
};

template <class search_string, class... components>
struct SIGet<SimInterfaceContainer<SISingle<search_string>, components...>, search_string> {
	using Parent = SimInterfaceContainer<SISingle<search_string>, components...>;
	using Result = SISingle<search_string>;

	static constexpr Result &get(Parent &p)
	{
		return p.first_;
	}
};

template <class search_string, class... inner_components, class... components>
struct SIGet<SimInterfaceContainer<SIComposite<search_string, inner_components...>, components...>, search_string> {
	using Parent = SimInterfaceContainer<SIComposite<search_string, inner_components...>, components...>;
	using Result = SIComposite<search_string, inner_components...>;

	static constexpr Result &get(Parent &p)
	{
		return p.first_;
	}
};


template <class name, class search_string, class... components>
struct SIGet<SIComposite<name, components...>, search_string> {
	using Parent = SIComposite<name, components...>;
	using Container = typename Parent::Container;
	using Next = SIGet<Container, search_string>;
	using Result = typename Next::Result;

	static constexpr Result &get(Parent &p)
	{
		return Next::get(p.components_);
	}
};


template <class IF, class search_string, class... search_strings>
struct SIGetR {
	using Parent = IF;
	using Next = SIGet<Parent, search_string>;
	using NextComp = SIGetR<typename Next::Result, search_strings...>;
	using Result = typename NextComp::Result;

	static constexpr Result &get(Parent &p)
	{
		return NextComp::get(Next::get(p));
	}
};

template <class IF, class search_string>
struct SIGetR<IF, search_string> {
	using Parent = IF;
	using Next = SIGet<Parent, search_string>;
	using Result = typename Next::Result;

	static constexpr Result &get(Parent &p)
	{
		return Next::get(p);
	}
};

template <class outer_name, class component0, class... components, class... search_strings>
struct SIGet<SIComposite<outer_name, component0, components...>, SStringList<search_strings...> > {
	using Parent = SIComposite<outer_name, component0, components...>;
	using Next = SIGetR<Parent, search_strings...>;
	using Result = typename Next::Result;

	static constexpr Result &get(Parent &p)
	{
		return Next::get(p);
	}
};

}

#endif // TRAVERSAL_H
