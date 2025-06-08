/*
 * c_api.cc
 *
 *  Created on: Apr 5, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/connectible.h>

#include "c_api/rootnamespace.h"
#include "c_api/typedcvalue.h"

#include "construction/library.h"
#include "construction/interfacecompatexception.h"

#include "entity/entity.h"

#include "representation/txt/commands.h"

#include "util/iterable.h"

#include "representation/structural/builder/structure.h"
#include "representation/structural/builder/complexport.h"

#include <array>
#include <functional>
#include <cassert>
#include <string>

extern "C" {
#include <rhdl/construction/c/functions.h>
#include <rhdl/construction/c/error.h>
}

using rhdl::Connectible;
using rhdl::ConstructionException;
using rhdl::InterfaceCompatException;
using rhdl::Wrapper;
using rhdl::recover;
using rhdl::c_ptr;
using rhdl::structural::builder::Structure;
using rhdl::structural::builder::Port;
using rhdl::structural::builder::makeStructure;

const char *const rhdl_type_names[RHDL_NUM_TYPES] = {
		"ENTITY", "REPTYPE", "REPRESENTATION",
		"TRANSFORMATION", "INTERFACE",
		"RDHL_STRUCTURE", "OPTION", "NAMESPACE",
		"DIRECTION",
		"CSTRING", "I64", "U64",
		"OBJREF",
		"STRUCTURE_HANDLE", "CONNECTOR"
};

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
inline int64_t cerror_return<int64_t>(int)
{
	return -1;
}

template <>
inline uint64_t cerror_return<uint64_t>(int)
{
	return -1;
}

template <>
inline rhdl_direction cerror_return<rhdl_direction>(int)
{
	return RHDL_INVALID_DIRECTION;
}

template <>
inline rhdl_reptype cerror_return<rhdl_reptype>(int)
{
	return RHDL_INVALID_REPTYPE;
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

		std::cerr << "Got unexpected exception: " << e.what() << std::endl;
		assert (0);
	}

	return cerror_return<RTYPE>(-1);
}

enum Errorcode rhdl_errno()
{
	return static_cast<Errorcode>(lastException.errorcode());
}

const char *rhdl_errstr()
{
	return lastException.what();
}

static rhdl::Namespace &recover_namespace(rhdl_namespace_t *ns)
{
	return ns ? rhdl::Namespace::recover(ns) : rhdl::rootNamespace;
}

static rhdl::Namespace &recover_namespace_for_entity(rhdl_namespace_t *ns)
{
	return ns ?	rhdl::Namespace::recover(ns) : *rhdl::defaultLib;
}

rhdl_namespace_t *rhdl_namespace(rhdl_namespace_t* ns, const char *name)
{
	auto f = [=]() {
		const rhdl::Namespace &base = recover_namespace(ns);

		if (!name)
			return base.c_ptr();

		const rhdl::Namespace &result = base.at(name);
		return result.c_ptr();
	};
	return cerror<rhdl_namespace_t *, 2>(f, std::array<int, 2>{E_WRONG_OBJECT_TYPE, E_NO_SUCH_MEMBER});
}

rhdl_namespace_t *rhdl_create_namespace(rhdl_namespace_t* ns, const char *name)
{
	auto f = [=]() {
		rhdl::Namespace &base = recover_namespace(ns);

		const rhdl::Namespace &result =
				base.add(std::make_unique<rhdl::Namespace>(name));

		return result.c_ptr();
	};
	return cerror<rhdl_namespace_t *, 2>(f, std::array<int, 2>{E_NO_SUCH_MEMBER, E_MEMBER_EXISTS});
}

rhdl_entity_t *rhdl_entity(rhdl_namespace_t* ns, const char *name)
{
	//namespaces are not implemented yet
	assert (ns == nullptr);

	auto f = [=]() {
		const rhdl::Entity &entity = recover_namespace_for_entity(ns).at(name);
		return entity.c_ptr();
	};
	return cerror<rhdl_entity_t *, 2>(f, std::array<int, 2>{E_WRONG_OBJECT_TYPE, E_NO_SUCH_MEMBER});
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
	try {
		rhdl::Namespace &ns = recover_namespace_for_entity(nspace);
		const auto &structure = *makeStructure(ns, entity_name,
				static_cast<rhdl::Structure::Mode>(mode)).release();
		return structure.c_ptr();
	}
	catch (const ConstructionException &e) {
		switch (e.errorcode()) {
		case rhdl::Errorcode::E_MEMBER_EXISTS:
		case rhdl::Errorcode::E_NO_SUCH_MEMBER:
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
		auto &cpp = recover<Structure>(structure);
		cpp.finalize();
		delete &cpp;
		return 0;
	};

	return cerror<int, 2>(f, {E_NETLIST_CONTAINS_CYCLES, E_EMPTY_INTERFACE});
}

int rhdl_abort_structure(rhdl_structure_t *structure)
{
	auto f = [=](){
		auto &cpp = recover<Structure>(structure);
		cpp.abort();
		delete &cpp;
		return 0;
	};

	return cerror<int, 0>(f, {});
}


rhdl_connector_t *rhdl_component(rhdl_structure_t *structure, rhdl_entity_t *entity)
{
	auto f = [=]() {
		auto &cpp_sh = recover<Structure>(structure);
		auto &cpp_entity = rhdl::Entity::recover(entity);
		const Port &cpp_component = cpp_sh.add(cpp_entity);
		return cpp_component.c_ptr();
	};

	return cerror<rhdl_connector_t *, 1>(f, {E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY});
}

rhdl_connector_t *rhdl_select(rhdl_connector_t *connector, const char *iface_name)
{
	std::function<rhdl_connector_t *()> f = [=]() {
		auto &cpp_handle = recover<Port>(connector);
		const auto &selected_handle = cpp_handle[iface_name];
		return selected_handle.c_ptr();
	};

	return cerror<rhdl_connector_t *, 2>(f, {E_NO_SUCH_INTERFACE, E_CANNOT_GET_ANONYMOUS_INTERFACE});
}

int rhdl_connect(rhdl_connector_t *from, rhdl_connector_t *to)
{
	try {
		auto &cpp_from = recover<Port>(from);
		auto &cpp_to = recover<Port>(to);

		if (cpp_from.element().isTheStructure() &&
				cpp_to.element().isTheStructure()) {
			throw ConstructionException(rhdl::Errorcode::E_ILLEGAL_PASSTHROUGH);
		}

		Port::connect(cpp_from, cpp_to);
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
		const rhdl::Entity &entity = rhdl::defaultLib -> at(entity_name);
		auto *commands = entity.getRepresentation<rhdl::txt::Commands>();
		assert (commands);
		std::cout << *commands;
		return 0;
	};

	return cerror<int, 1>(f, std::array<int, 1>{E_NO_SUCH_MEMBER});
}

const rhdl_object * rhdlo_get(const rhdl_object_t *o, const char *member) {
	auto f = [=]() {
		const rhdl::CObject &cobject = o ? recover<rhdl::CObject>(o) : rhdl::rootNamespace;
		const rhdl::CObject &result = member ? cobject.at(member) : cobject;

		return c_ptr(result);
	};

	return cerror<const rhdl_object *, 1>(f, std::array<int, 1>
			{E_NO_SUCH_MEMBER});
}

template <class VALUE_TYPE>
static VALUE_TYPE rhdl_read_value(const rhdl_object_t *o) {
	auto f = [=]() {
		const auto &co = rhdl::recover<rhdl::CObject>(o);
		return static_cast<const VALUE_TYPE>(co);
	};

	auto result = cerror<VALUE_TYPE, 1>(f, std::array<int, 1>{E_WRONG_VALUE_TYPE});
	return result;
}

int rhdlo_has_value(const rhdl_object_t *o) {
	auto f = [=]() {
		const auto &co = rhdl::recover<rhdl::CObject>(o);
		return co.isValue();
	};

	return cerror<int, 0>(f, std::array<int, 0>{});
}

const char* rhdlo_read_cstring(const rhdl_object_t *o)
{
	return rhdl_read_value<const char *>(o);
}

int64_t rhdlo_read_i64(const rhdl_object_t *o) {
	return rhdl_read_value<int64_t>(o);
}

uint64_t rhdlo_read_u64(const rhdl_object_t *o) {
	return rhdl_read_value<uint64_t>(o);
}

rhdl_object_t* rhdlo_read_objref(const rhdl_object_t *o)
{
	return c_ptr(*rhdl_read_value<const rhdl::CObject *>(o));
}

rhdl_direction rhdlo_read_direction(const rhdl_object_t *o) {
	return rhdl_read_value<rhdl_direction>(o);
}

enum rhdl_reptype rhdlo_read_reptype(const rhdl_object_t *o) {
	return rhdl_read_value<rhdl_reptype>(o);
}

rhdl_object_t *rhdlo_transform(
		rhdl_object_t *representation, rhdl_object_t *transformation,
		const char *result_name)
{
	auto f = [=]() {
		const rhdl::Representation &rep = recover<rhdl::CObject>(representation);
		const rhdl::Transformation &trans = recover<rhdl::CObject>(transformation);

		rhdl_representation_t *dst = rhdl_transform(rep.c_ptr(), trans.c_ptr(), result_name);

		if (!dst)
			return static_cast<rhdl_object_t *>(nullptr);

		const rhdl::CObject &result = rhdl::Representation::recover(dst);
		return c_ptr(result);
	};

	return cerror<rhdl_object_t *, 2>(f, std::array<int, 2>{E_WRONG_OBJECT_TYPE, E_TRANSFORMATION_NOT_APPLICABLE});
}

rhdl_representation_t* rhdl_transform(rhdl_representation_t *representation,
		rhdl_transformation_t *transformation, const char *result_name)
{
	auto f = [=]() {
		const rhdl::Representation &rep = rhdl::Representation::recover(representation);
		const rhdl::Transformation &trans = rhdl::Transformation::recover(transformation);

		std::string name = result_name ? result_name : "";

		return rep.entity().addRepresentation(trans.execute(rep, name)).c_ptr();
	};

	return cerror<rhdl_representation_t *, 2>(f, std::array<int, 2>{E_WRONG_OBJECT_TYPE, E_TRANSFORMATION_NOT_APPLICABLE});
}

rhdl_entity_t* rhdlo_entity(rhdl_object_t *o)
{
	auto f = [=](){
		const rhdl::Entity &e = recover<rhdl::CObject>(o);
		return e.c_ptr();
	};

	return cerror<rhdl_entity_t *, 1>(f, std::array<int, 1>{E_WRONG_OBJECT_TYPE});
}

