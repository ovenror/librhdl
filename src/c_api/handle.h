/*
 * handle.h
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#ifndef C_API_HANDLE_H_
#define C_API_HANDLE_H_

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/porthandle.h>
#include "c_api/wrapper.h"

#include <memory>
#include <string>

namespace rhdl {

class Context;
class Structure;
class StructureConnector;
class Connector;

namespace structural::builder { class Port; }

class Handle {
public:
	Handle &select(const std::string &name) const;
	void connect(Handle &h);

protected:
	void pt() const;

private:
	friend class Context;

	Handle(Context &context, structural::builder::Port &port);
	virtual ~Handle();

	void updateC();

	Context &context_;
	PortHandle ph_;

public:
	using C_Struct = rhdl_connector;

private:
	friend class Wrapper<Handle>;
	static constexpr unsigned long C_ID = 0x49D1E5;
	Wrapper<Handle> c_;
};

} /* namespace rhdl */

#endif /* C_API_HANDLE_H_ */
