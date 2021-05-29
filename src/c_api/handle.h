/*
 * handle.h
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#ifndef C_API_HANDLE_H_
#define C_API_HANDLE_H_

#include <rhdl/construction/c/types.h>
#include "c_api/wrapper.h"

#include <memory>
#include <string>

namespace rhdl {

class Context;
class Structure;
class EntityInterface;
class PartInterface;
class PartHandle;

template <class T> class TypedHandle;

class Handle {
public:
	virtual Handle &select(const std::string &name) const = 0;
	virtual void connect(const Handle &h) const = 0;

protected:
	void pt() const;

private:
	Handle(Context &context);
	virtual ~Handle();

	virtual void getConnectedFrom(const TypedHandle<Structure> &) const = 0;
	virtual void getConnectedFrom(const TypedHandle<EntityInterface> &) const = 0;
	virtual void getConnectedFrom(const TypedHandle<PartHandle> &) const = 0;
	virtual void getConnectedFrom(const TypedHandle<PartInterface> &) const = 0;

	Context &context_;

public:
	using C_Struct = rhdl_connector;

private:
	friend class TypedHandle<Structure>;
	friend class TypedHandle<EntityInterface>;
	friend class TypedHandle<PartHandle>;
	friend class TypedHandle<PartInterface>;

	friend class Context;
	friend class Wrapper<Handle>;
	static constexpr unsigned long C_ID = 0x49D1E5;
	Wrapper<Handle> c_;
};

} /* namespace rhdl */

#endif /* C_API_HANDLE_H_ */
