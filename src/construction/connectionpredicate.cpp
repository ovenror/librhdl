/*
 * connectionpredicate.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include "construction/connectionpredicate.h"

namespace rhdl {

ConnectionPredicate::ConnectionPredicate(bool samedir,
	bool check_first_dir, SingleDirection first_dir)
:
	samedir_(samedir), check_first_dir_(check_first_dir), first_dir_(first_dir)
{
	//std::cerr << "PDEFCNST(samedir=" << samedir << ", check_fd=" << check_first_dir << ", first_dir=" << first_dir << ")" << std::endl;
}

ConnectionPredicate ConnectionPredicate::reversed() const {
	if (samedir_)
		return *this;
	else
		return ConnectionPredicate(
				this -> samedir_, this -> check_first_dir_,
				rhdl::reversed(this -> first_dir_));
}

ConnectionPredicate ConnectionPredicate::entity_corrected() const {
	ConnectionPredicate result(*this);

	if (samedir_)
		return result;

	if (check_first_dir_)
		result.first_dir_ = rhdl::reversed(first_dir_);

	result.samedir_ = true;
	return result;
}

}

