/*
 * compositedirection.h
 *
 *  Created on: Sep 9, 2021
 *      Author: ovenror
 */

#ifndef SRC_INTERFACE_COMPOSITEDIRECTION_H_
#define SRC_INTERFACE_COMPOSITEDIRECTION_H_

#include "direction.h"
#include <ostream>

namespace rhdl {


class CompositeDirection {
public:
	CompositeDirection();
	CompositeDirection(bool out, bool in);
	CompositeDirection(SingleDirection dir);

	operator SingleDirection() const;

	CompositeDirection &operator|=(const CompositeDirection &rhs);
	CompositeDirection operator|(const CompositeDirection &rhs) const;
	CompositeDirection &operator&=(const CompositeDirection &rhs);
	CompositeDirection operator&(const CompositeDirection &rhs) const;

	bool operator==(const CompositeDirection &rhs) const;
	bool operator!=(const CompositeDirection &rhs) const {return !(*this == rhs);}
	CompositeDirection complement() const;

	//operator bool() {return !mixed();}

	bool xout() const {return code_ == vout();}
	bool xin() const {return code_ == vin();}
	bool out() const {return code_ & vout();}
	bool in() const {return code_ & vin();}
	bool free() const {return !(out() | in());}
	bool mixed() const {return out() && in();}
	bool single() const {return out() != in();}

private:
	CompositeDirection(int code);

	friend std::ostream &operator<<(std::ostream &os, CompositeDirection dir);
	void decode(std::ostream &os);
	constexpr int value(SingleDirection dir) const {return 1 << static_cast<int>(dir);}
	constexpr int vin() const {return value(SingleDirection::IN);}
	constexpr int vout() const {return value(SingleDirection::OUT);}

	int code_;
};

std::ostream &operator<<(std::ostream &os, CompositeDirection dir);

} /* namespace rhdl */

#endif /* SRC_INTERFACE_COMPOSITEDIRECTION_H_ */
