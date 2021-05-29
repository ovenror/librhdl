#ifndef SISINGLE_BASE_H
#define SISINGLE_BASE_H

#include "siminterface.h"
#include <boost/circular_buffer.hpp>

namespace rhdl {

class SISingle_Base
{
public:
	using IMap = std::map<const ISingle *, SISingle_Base*>;

	SISingle_Base();

	void push(bool value)
	{
		recent_.push_front(value);
	}

	bool operator[] (size_t age)
	{
		assert(recent_[0] == state_);
		return recent_[age];
	}

	void record()
	{
		push(state_);
	}

	void init_history(const std::vector<bool> &values);

	bool state_;

private:
	void history_reserve(size_t capa);

	boost::circular_buffer<bool> recent_;
};


}

#endif // SISINGLE_BASE_H
