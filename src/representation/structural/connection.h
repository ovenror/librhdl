/*
 * flatconnection.h
 *
 *  Created on: Jun 14, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_CONNECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_CONNECTION_H_

#include "connectionbase.h"
#include "port.h"

#include <vector>

namespace rhdl {

class ISingle;

namespace structural {

class Connection;

class Connection : public ConnectionBase<std::vector<Port>> {
public:
	Connection(std::vector<Port> ports);

	void setOpen() {open_ = true;}
	bool open() const {return open_;}
	void computeClosings();

private:
	bool open_ = false;
};

std::ostream &operator<<(std::ostream &os, Connection &c);

}} /* namespace rhdl::structural */

#endif /* SRC_REPRESENTATION_STRUCTURAL_CONNECTION_H_ */
