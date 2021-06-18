#ifndef LIBRARY_H
#define LIBRARY_H

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/component.h>
#include "construction/internalstructure.h"

#include "lib/lib.h"

#include "util/dictionary.h"
#include "c_api/wrapper.h"

#include <utility>
#include <map>
#include <memory>


namespace rhdl {

class Entity;

class Library {
public:
	Library();
	~Library();

	Entity &regist(std::unique_ptr<Entity> &&e);
	bool contains(const std::string &name);
	Entity &at(const std::string &name);

	const Dictionary<std::unique_ptr<Entity>> &entities() const {return entities_;}

private:
	Entity &regist_internal(std::unique_ptr<Entity> &&entity);

	Dictionary<std::unique_ptr<Entity>> entities_;

public:
	using C_Struct = rhdl_namespace_struct;

private:
	friend class Wrapper<Library>;
	static constexpr unsigned long C_ID = 0x1185A51E;
	Wrapper<Library> c_;
};

using Part = Component;
using Ent = InternalStructure;

extern Library defaultLib;

}

#endif // LIBRARY_H
