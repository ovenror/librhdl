#ifndef MARRAY_ITERATE_H
#define MARRAY_ITERATE_H

#include <iterator>
#include <cassert>

namespace marray {

using size_type = unsigned int;

template <class MARRAY, size_type DIM>
struct EIteratorGenerator;

template <class MARRAY, size_type DIM>
class eiterator : std::iterator<
		std::forward_iterator_tag, typename MARRAY::element>
{
public:
	using Super = std::iterator<std::output_iterator_tag, typename MARRAY::element>;

	using ITERATOR = typename MARRAY::iterator;
	using SUBARRAY = typename ITERATOR::value_type;
	using GENERATOR = EIteratorGenerator<SUBARRAY, DIM-1>;
	using SUB_EITERATOR = typename GENERATOR::RESULT;

	eiterator() {} //unusable

	eiterator(ITERATOR iter, ITERATOR end)
		: iterator_(iter), end_(end)
	{
		if (iter != end)
			initSubiterator();
	}

	eiterator& operator++()
	{
		assert(sub_eiterator_ != sub_end_);
		assert(iterator_ != end_);

		if (++sub_eiterator_ != sub_end_)
			return *this;

		if (++iterator_ == end_) {
			return *this;
		}

		initSubiterator();
		return *this;
	}

	eiterator operator++(int) {eiterator retval = *this; ++(*this); return retval;}

	bool operator==(const eiterator &other) const
	{
		if (iterator_ != other.iterator_)
			return false;

		if (iterator_ == end_)
			return true;

		return sub_eiterator_ == other.sub_eiterator_;
	}

	bool operator!=(const eiterator &other) const {return !(*this == other);}

	typename Super::reference operator*() const
	{
		assert (iterator_ != end_);
		assert (sub_eiterator_ != sub_end_);
		return *sub_eiterator_;
	}

private:
	void initSubiterator() {
		sub_eiterator_ = GENERATOR::generate(iterator_ -> begin(), iterator_ -> end());
		sub_end_ = GENERATOR::generate(iterator_ -> end(), iterator_ -> end());
	}

	ITERATOR iterator_;
	ITERATOR end_;
	SUB_EITERATOR sub_eiterator_;
	SUB_EITERATOR sub_end_;
};

template <class MARRAY>
auto ebegin(MARRAY &arr)
{
	return EIteratorGenerator<MARRAY, MARRAY::dimensionality>::generate(arr.begin(), arr.end());
}

template <class MARRAY>
auto eend(MARRAY &arr)
{
	return EIteratorGenerator<MARRAY, MARRAY::dimensionality>::generate(arr.end(), arr.end());
}

template <class MARRAY>
struct EWrap {
	EWrap(MARRAY &arr) : arr_(arr) {}

	auto begin() {return ebegin(arr_);}
	auto end() {return eend(arr_);}

	MARRAY &arr_;
};

template <class MARRAY>
EWrap<MARRAY> ewrap(MARRAY &arr) {return EWrap<MARRAY>(arr);}

template <class MARRAY, size_type DIM>
struct EIteratorGenerator {
	using RESULT = eiterator<MARRAY, DIM>;

	static RESULT generate(typename MARRAY::iterator iter, typename MARRAY::iterator end) {return RESULT(iter, end);}
};

template <class MARRAY>
struct EIteratorGenerator<MARRAY, 1> {
	using RESULT = typename MARRAY::iterator;

	static auto generate(typename MARRAY::iterator iter, typename MARRAY::iterator end) {std::ignore = end; return iter;}
};

}

#endif // MARRAY_ITERATE_H
