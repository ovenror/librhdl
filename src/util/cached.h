#ifndef CACHED_H
#define CACHED_H

#include "util.h"
#include <utility>

template <class RESULT_TYPE, class COMPUTER, int ID = 0>
class Cached_Base {
public:
	template <class... Args>
	Cached_Base(COMPUTER compute, Args&&... args)
		: compute_(compute), return_value_(std::forward<Args>(args)...),
		  invalid_(true)
	{}

	Cached_Base(Cached_Base &&moved)
		: compute_(moved.compute_),
		  return_value_(std::move(moved.return_value_)),
		  invalid_(moved.invalid_)
	{}

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
	mutable RESULT_TYPE return_value_;
	mutable bool invalid_;
};

template <class RESULT_TYPE, class CONTAINER = None, int ID = 0>
class Cached : public Cached_Base<RESULT_TYPE, void (CONTAINER::*)(RESULT_TYPE &result) const, ID> {
public:
	using Computer = void (CONTAINER::*)(RESULT_TYPE &result) const;
	using Super =  Cached_Base<RESULT_TYPE, void (CONTAINER::*)(RESULT_TYPE &result) const, ID>;

	template <class... Args>
	Cached(CONTAINER &container, Computer compute, Args&&... args)
		: container_(container), Super(compute, std::forward(args)...) {}

	Cached(Cached &&moved, CONTAINER &newContainer)
		: Super(std::move(moved)), container_(newContainer) {}

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
class Cached<RESULT_TYPE, None, ID> : public Cached_Base<RESULT_TYPE, void (*)(RESULT_TYPE &result), ID> {
	using Computer = void (*)(RESULT_TYPE &result);
	using Super = Cached_Base<RESULT_TYPE, void (*)(RESULT_TYPE &result), ID>;

	template <class... Args>
	Cached(Computer compute, Args&&... args) : Super(compute, std::forward(args)...) {}

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
