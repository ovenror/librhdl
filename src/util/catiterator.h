#ifndef CATITERATOR_H
#define CATITERATOR_H

#include <iterator>
#include <cassert>
#include <type_traits>

template <class CONTAINER_ITERATOR>
class CatIterator
{
	template <class Container> struct GetElementIterator;
	using Container = typename std::remove_reference<decltype(*std::declval<CONTAINER_ITERATOR>())>::type;
	using ElementIterator = typename GetElementIterator<Container>::type;
	using Element = typename ElementIterator::value_type;

public:
	using value_type = Element;
	using reference = typename ElementIterator::reference;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = typename ElementIterator::difference_type;

	CatIterator() : CatIterator(CONTAINER_ITERATOR(), CONTAINER_ITERATOR()) {}
	CatIterator(std::pair<CONTAINER_ITERATOR, CONTAINER_ITERATOR> iterator_pair) : CatIterator(iterator_pair.first, iterator_pair.second) {}
	CatIterator(CONTAINER_ITERATOR begin, CONTAINER_ITERATOR end) : icontainer_(begin), containerEnd_(end)
	{
		startIteratingCurrentContainer();
	}

	void startIteratingCurrentContainer()
	{
		while (icontainer_ != containerEnd_) {
			ielement_ = icontainer_ -> begin();

			if (ielement_ != icontainer_ -> end())
				break;

			++icontainer_;
		}
	}

	CatIterator& operator++()
	{
		if (++ielement_ != icontainer_ -> end())
			return *this;

		++icontainer_;
		startIteratingCurrentContainer();
		return *this;
	}

	CatIterator operator++(int) {CatIterator retval = *this; ++(*this); return retval;}

	bool operator==(const CatIterator &other) const
	{
		if (icontainer_ != other.icontainer_)
			return false;

		if (icontainer_ == containerEnd_)
			return true;

		return ielement_ == other.ielement_;
	}

	bool operator!=(const CatIterator &other) const {return !(*this == other);}

	reference operator*() const
	{
		assert (icontainer_ != containerEnd_);
		assert (ielement_ != icontainer_ -> end());
		return *ielement_;
	}

	Element *operator->() const {
		return &*ielement_;
	}

	difference_type distance(CatIterator next) {
		if (next.icontainer_ == icontainer_) {
			if (icontainer_ == containerEnd_)
				return 0;

			return std::distance(ielement_, next.ielement_);
		}

		CONTAINER_ITERATOR iRunningContainer = icontainer_;
		++iRunningContainer;

		difference_type accu = std::distance(ielement_, icontainer_ -> end());

		while (iRunningContainer != next.icontainer_) {
			accu += std::distance(iRunningContainer -> begin(), iRunningContainer -> end());
			++iRunningContainer;
		}

		if (iRunningContainer == containerEnd_)
			return accu;

		return accu + std::distance(iRunningContainer -> begin(), next.ielement_);
	}

private:
	CONTAINER_ITERATOR icontainer_;
	CONTAINER_ITERATOR containerEnd_;
	ElementIterator ielement_;
};

template <class CONTAINER_ITERATOR>
struct std::iterator_traits<CatIterator<CONTAINER_ITERATOR> > {
	using Iterator = CatIterator<CONTAINER_ITERATOR>;

	using value_tyoe = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using iterator_category = typename Iterator::iterator_category;
	using difference_type = typename Iterator::difference_type;
};

template<typename CI>
typename CatIterator<CI>::difference_type distance(CatIterator<CI> first, CatIterator<CI> last)
{
	return first.distance(last);
}

template <class CONTAINER_ITERATOR>
struct CatGenerator {
	using iterator = CatIterator<CONTAINER_ITERATOR>;
	using value_type = typename iterator::value_type;

	CatGenerator(const CatGenerator &) = default;
	CatGenerator &operator=(const CatGenerator &) = default;

	CatGenerator(std::pair<CONTAINER_ITERATOR, CONTAINER_ITERATOR> iterator_pair) : CatGenerator(iterator_pair.first, iterator_pair.second) {}
	CatGenerator(CONTAINER_ITERATOR begin, CONTAINER_ITERATOR end) : begin_(begin), end_(end) {}

	auto begin() {return iterator(begin_, end_);}
	auto end() {return iterator(end_, end_);}

private:
	CONTAINER_ITERATOR begin_;
	CONTAINER_ITERATOR end_;
};

template <class CONTAINER_ITERATOR>
template <class Container>
struct CatIterator<CONTAINER_ITERATOR>::GetElementIterator {
	using type = typename Container::iterator;
};

template <class CONTAINER_ITERATOR>
template <class Container>
struct CatIterator<CONTAINER_ITERATOR>::GetElementIterator<const Container> {
	using type = typename Container::const_iterator;
};

#endif // CATITERATOR_H
