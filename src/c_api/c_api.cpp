/*
 * c_api.cc
 *
 *  Created on: Apr 5, 2021
 *      Author: ovenror
 */

#include "handle.h"
#include "structurehandle.h"

#include "construction/library.h"
#include "construction/interfacecompatexception.h"

#include "entity/entity.h"

#include "representation/txt/commands.h"

#include "util/iterable.h"

#include <array>
#include <functional>
#include <cassert>
#include <string>

extern "C" {
#include <rhdl/construction/c/functions.h>
#include <rhdl/construction/c/error.h>
}

using rhdl::Handle;
using rhdl::StructureHandle;
using rhdl::Connectible;
using rhdl::ConstructionException;
using rhdl::InterfaceCompatException;
using rhdl::Wrapper;
using rhdl::recover;
using rhdl::c_ptr;

static ConstructionException lastException(rhdl::Errorcode::E_NO_ERROR);

static void except(const ConstructionException &e)
{
	lastException = e;
}

template <class RTYPE>
inline RTYPE cerror_return(int ec)
{
	return nullptr;
}

template <>
inline int cerror_return<int>(int ec)
{
	return static_cast<int>(ec);
}

template <class RTYPE, unsigned NEXPECTED>
static inline RTYPE cerror(std::function<RTYPE()> f, std::array<int, NEXPECTED> expected)
{
	try {
		return f();
	}
	catch (ConstructionException &e) {
		int ec = static_cast<int>(e.errorcode());

		switch (ec) {
		case E_UNKNOWN_STRUCT:
		case E_WRONG_STRUCT_TYPE:
		case E_INVALID_HANDLE:
			except(e);
			return cerror_return<RTYPE>(ec);
		}

		for (int exp : expected) {
			if (ec == exp) {
				except(e);
				return cerror_return<RTYPE>(ec);
			}
		}

		assert (0);
	}

	return cerror_return<RTYPE>(-1);
}

int rhdl_errno()
{
	return static_cast<int>(lastException.errorcode());
}

const char *rhdl_errstr()
{
	return lastException.what();
}

rhdl_namespace_t *rhdl_namespace(rhdl_namespace_t* ns, const char *name)
{
	//namespaces are not implemented yet
	assert (ns == nullptr);
	assert (name == nullptr);

	auto f = [=]() {
		return c_ptr(rhdl::defaultLib);
	};
	return cerror<rhdl_namespace_t *, 0>(f, std::array<int, 0>{});
}

rhdl_entity_t *rhdl_entity(rhdl_namespace_t* ns, const char *name)
{
	//namespaces are not implemented yet
	assert (ns == nullptr);

	auto f = [=]() {
		return c_ptr(rhdl::defaultLib.at(name));
	};
	return cerror<rhdl_entity_t *, 1>(f, std::array<int, 1>{E_NO_SUCH_ENTITY});
}

rhdl_iface_t *rhdl_iface(rhdl_iface_t *iface, const char *name)
{
	auto f = [=](){

		auto &cpp_iface = recover<rhdl::Interface>(iface);
		auto *sub = cpp_iface[name];

		if (!sub)
			throw ConstructionException(rhdl::Errorcode::E_NO_SUCH_INTERFACE);

		return c_ptr(*sub);
	};

	return cerror<rhdl_iface_t *, 2>(f, {E_NO_SUCH_INTERFACE, E_CANNOT_GET_ANONYMOUS_INTERFACE});
}

rhdl_structure_t *rhdl_begin_structure(rhdl_namespace_t *nspace, const char *entity_name, unsigned int mode)
{
	//namespaces are not implemented yet
	assert (nspace == nullptr);

	try {
		return c_ptr(*new StructureHandle(entity_name, mode));
	}
	catch (const ConstructionException &e) {
		switch (e.errorcode()) {
		case rhdl::Errorcode::E_ENTITY_EXISTS:
		case rhdl::Errorcode::E_NO_SUCH_ENTITY:
			except(e);
			return nullptr;
		default:
			assert (0);
		}
	}

	return nullptr;
}

int rhdl_finish_structure(rhdl_structure_t *structure)
{
	auto f = [=](){
		auto &cpp = recover<StructureHandle>(structure);
		cpp.finalize();
		delete &cpp;
		return 0;
	};

	return cerror<int, 2>(f, {E_NETLIST_CONTAINS_CYCLES, E_EMPTY_INTERFACE});
}

int rhdl_abort_structure(rhdl_structure_t *structure)
{
	auto f = [=](){
		auto &cpp = recover<StructureHandle>(structure);
		cpp.abort();
		delete &cpp;
		return 0;
	};

	return cerror<int, 0>(f, {});
}


rhdl_connector_t *rhdl_component(rhdl_structure_t *structure, rhdl_entity_t *entity)
{
	auto f = [=]() {
		auto &cpp_sh = recover<StructureHandle>(structure);
		auto &cpp_entity = recover<rhdl::Entity>(entity);
		auto &cpp_component = cpp_sh.makeComponent(cpp_entity);
		return c_ptr(cpp_component);
	};

	return cerror<rhdl_connector_t *, 1>(f, {E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY});
}

rhdl_connector_t *rhdl_select(rhdl_connector_t *connector, const char *iface_name)
{
	std::function<rhdl_connector_t *()> f = [=]() {
		auto &cpp_handle = recover<Handle>(connector);
		auto &selected_handle = cpp_handle.select(iface_name);
		return c_ptr(selected_handle);
	};

	return cerror<rhdl_connector_t *, 2>(f, {E_NO_SUCH_INTERFACE, E_CANNOT_GET_ANONYMOUS_INTERFACE});
}

int rhdl_connect(rhdl_connector_t *from, rhdl_connector_t *to)
{
	try {
		auto &cpp_from = recover<Handle>(from);
		auto &cpp_to = recover<Handle>(to);
		cpp_from.connect(cpp_to);
		return 0;
	}
	catch (const InterfaceCompatException &e) {
		except(e);
		return static_cast<int>(e.errorcode());
	}
	catch (const ConstructionException &e) {
		switch (e.errorcode()) {
		case rhdl::Errorcode::E_INVALID_HANDLE:
		case rhdl::Errorcode::E_UNKNOWN_STRUCT:
		case rhdl::Errorcode::E_COMPATIBLE_INTERFACES_NOT_FOUND:
		case rhdl::Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES:
		case rhdl::Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY:
		case rhdl::Errorcode::E_ILLEGAL_RECONNECTION:
		case rhdl::Errorcode::E_ILLEGAL_PASSTHROUGH:
			except(e);
			return static_cast<int>(e.errorcode());
		default:
			assert (0);
		}
	}

	return -1;
}

int rhdl_print_commands(const char *entity_name) {
	auto f = [=]() {
		auto &entity = rhdl::defaultLib.at(entity_name);
		auto *commands = entity.getRepresentation<rhdl::txt::Commands>();
		assert (commands);
		std::cout << *commands;
		return 0;
	};

	return cerror<int, 1>(f, std::array<int, 1>{E_NO_SUCH_ENTITY});
}
