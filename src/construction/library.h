#ifndef LIBRARY_H
#define LIBRARY_H

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/component.h>

#include "construction/internalstructure.h"
#include "lib/lib.h"
#include "c_api/typedcobject.h"
#include "util/lexicalpointingdictionary.h"

#include <utility>
#include <map>
#include <memory>

namespace rhdl {

class Entity;

class Library : public TypedCObject<Library, rhdl_namespace_struct> {
public:
	Library();
	virtual ~Library();

	Library &cast() override {return *this;}

	const Entity &regist(std::unique_ptr<Entity> &&e);
	bool contains(const std::string &name) const;

	const Entity& at(const char *name) const override;
	const Entity& at(const std::string &name) const override;

	virtual std::size_t size() const override {return entities_.size();}

	virtual const std::vector<const char*> &c_strings() const {return entities_.c_strings();}

	const Dictionary<Entity> &entities() const {return entities_;}

private:
	void setMembers_internal(const char *const *members) override;
	const Entity &regist_internal(std::unique_ptr<Entity> &&entity);

	LexicalPointingDictionary<Entity> entities_;
};

using Part = Component;
using Ent = InternalStructure;

extern Library *defaultLib;

}

#endif // LIBRARY_H
