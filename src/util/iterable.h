#ifndef ITERABLE_H
#define ITERABLE_H

#include <tuple>

template <class Iterator>
struct Iterable {
	using iterator = Iterator;

	Iterable(Iterator begin, Iterator end)
		: begin_(begin), end_(end)
	{}

	Iterable(const std::pair<Iterator, Iterator> &p)
		: begin_(p.first), end_(p.second)
	{}

	Iterator begin() {return begin_;}
	Iterator end() {return end_;}
	auto size() {return std::distance(begin_, end_);}

private:
	Iterator begin_;
	Iterator end_;
};

#endif // ITERABLE_H
