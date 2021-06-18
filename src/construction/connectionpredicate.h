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
	using Direction = SingleDirection;

	ConnectionPredicate(bool samedir = false, bool check_first_dir = false,
			Direction first_dir = Direction::OUT);

	static ConnectionPredicate ptp(bool directional = true) {return {false, directional, Direction::OUT};}
	static ConnectionPredicate pte(bool directional = true) {return {true, directional, Direction::OUT};}
	static ConnectionPredicate etp(bool directional = true) {return {true, directional, Direction::IN};}
	static ConnectionPredicate equal() {return pte(false);}

	ConnectionPredicate reversed() const;
	ConnectionPredicate entity_corrected() const;

	bool samedir_;
	bool check_first_dir_;
	SingleDirection first_dir_;
};

}

#endif /* CONSTRUCTION_CONNECTIONPREDICATE_H_ */
