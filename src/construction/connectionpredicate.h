/*
 * connectionpredicate.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef CONSTRUCTION_CONNECTIONPREDICATE_H_
#define CONSTRUCTION_CONNECTIONPREDICATE_H_

#include <interface/direction.h>

namespace rhdl {

struct ConnectionPredicate {
public:
	using Direction = InterfaceDirection;

	ConnectionPredicate(bool samedir = false, bool check_first_dir = false,
			Direction first_dir = Direction::OUT);

	static ConnectionPredicate ptp() {return {false, true, Direction::OUT};}
	static ConnectionPredicate pte() {return {true, true, Direction::OUT};}
	static ConnectionPredicate pte_nondir() {return {true, false, Direction::OUT};}
	static ConnectionPredicate ptp_nondir() {return {false, false, Direction::OUT};}

	ConnectionPredicate reversed() const;
	ConnectionPredicate entity_corrected() const;

	bool samedir_;
	bool check_first_dir_;
	InterfaceDirection first_dir_;
};

}

#endif /* CONSTRUCTION_CONNECTIONPREDICATE_H_ */
