#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "transformationtypeid.h"
#include "representation/representations.h"
#include "c_api/namespace.h"

#include <vector>
#include <memory>

namespace rhdl {

class Transformation;

class Transformations
{
public:
	static constexpr TransformationTypeID INVALID{Representations::INVALID, Representations::INVALID};

	Transformations();

	std::unique_ptr<const Transformation> next() {
		if (objects_.empty())
			return nullptr;

		auto retval = std::move(objects_.back());
		objects_.pop_back();
		return retval;
	}

private:
	template <class... types> struct Init;

	std::vector<std::unique_ptr<const Transformation>> objects_;
};


}

#endif // TRANSFORMATIONS_H
