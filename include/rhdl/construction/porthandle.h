/*
 * porthandle.h
 *
 *  Created on: Jun 5, 2022
 *      Author: ovenror
 */

#ifndef SRC_CONSTRUCTION_PORTHANDLE_H_
#define SRC_CONSTRUCTION_PORTHANDLE_H_

namespace rhdl {

namespace structural::builder {
class Port;
class ExistingPort;
}

class PortHandle {
public:
	PortHandle(structural::builder::Port &p);
	virtual ~PortHandle();

	void invalidate();
	void realizePort(structural::builder::ExistingPort &p);
	structural::builder::Port &port() const;

private:
	structural::builder::Port *port_;
};

} /* namespace rhdl */

#endif /* SRC_CONSTRUCTION_PORTHANDLE_H_ */
