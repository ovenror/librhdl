#ifndef CACHED_H
#define CACHED_H

#include "util.h"
#include <utility>

namespace detail {

template <class T>
struct Cached_Storage {
protected:
	template <class... Args>
	Cached_Storage(Args&&... args)
		: return_value_(std::forward<Args>(args)...) {}

	Cached_Storage(Cached_Storage &&moved)
		: return_value_(std::move(moved.return_value_)) {}

	mutable T return_value_;
};

template <class T>
struct Cached_Storage<T&> {
protected:
	Cached_Storage(T &ref) : return_value_(ref) {}
	Cached_Storage(Cached_Storage &&moved, T &ref)
		: return_value_(ref) {}

	T &return_value_;
};

template <class RESULT_TYPE, class COMPUTER, int ID = 0>
class Cached_Base : public Cached_Storage<RESULT_TYPE> {
	using Super = Cached_Storage<RESULT_TYPE>;

protected:
	template <class... Args>
	Cached_Base(COMPUTER compute, Args&&... args)
		: Super(std::forward<Args>(args)...), compute_(compute),
		  invalid_(true)
	{}

	Cached_Base(Cached_Base &&moved)
		: Super(static_cast<Super &&>(std::move(moved))), compute_(moved.compute_),
		  invalid_(moved.invalid_)
	{}

	Cached_Base(Cached_Base &&moved, RESULT_TYPE ref)
		: Super(static_cast<Super &&>(std::move(moved)), ref), compute_(moved.compute_),
		  invalid_(moved.invalid_)
	{}

public:
	void invalidate() const {
		invalid_ = true;
	}

	bool invalid() const {
		if (invalid_) {
			invalid_ = false;
			return true;
		}

		return false;
	}

protected:
	const COMPUTER compute_;
	mutable bool invalid_;
};

}

template <class RESULT_TYPE, class CONTAINER = None, int ID = 0>
class Cached : public detail::Cached_Base<RESULT_TYPE, void (CONTAINER::*)(RESULT_TYPE &result) const, ID> {
public:
	using Super =  detail::Cached_Base<RESULT_TYPE, void (CONTAINER::*)(RESULT_TYPE &result) const, ID>;
	using Computer = void (CONTAINER::*)(RESULT_TYPE &result) const;

	template <class... Args>
	Cached(CONTAINER &container, Computer compute, Args&&... args)
		: container_(container), Super(compute, std::forward<Args>(args)...) {}

	Cached(Cached &&moved, CONTAINER &newContainer)
		: Super(std::move(moved)), container_(newContainer) {}

	Cached(Cached &&moved, CONTAINER &newContainer, RESULT_TYPE ref)
		: Super(std::move(moved), ref), container_(newContainer) {}

	operator const RESULT_TYPE&() const {return *this();}

	const RESULT_TYPE &operator()() const {
		if (Super::invalid()) {
			(container_.*Super::compute_)(Super::return_value_);
		}
		return Super::return_value_;
	}

private:
	const CONTAINER &container_;
};


template <class RESULT_TYPE, int ID>
class Cached<RESULT_TYPE, None, ID> : public detail::Cached_Base<RESULT_TYPE, void (*)(RESULT_TYPE &result), ID> {
	using Computer = void (*)(RESULT_TYPE &result);
	using Super = detail::Cached_Base<RESULT_TYPE, void (*)(RESULT_TYPE &result), ID>;

	template <class... Args>
	Cached(Computer compute, Args&&... args) : Super(compute, std::forward<Args>(args)...) {}

	Cached(Cached &&moved)	: Super(std::move(moved)) {}

	operator const RESULT_TYPE&() const {return *this();}

	const RESULT_TYPE &operator()() const {
		if (Super::invalid()) {
			Super::compute_(Super::return_value_);
		}
		return Super::return_value_;
	}
};

#endif // CACHED_H
