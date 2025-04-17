#ifndef LIBRARY_H
#define LIBRARY_H

#include <rhdl/construction/c/types.h>

#include "lib/lib.h"
#include "c_api/namespace.h"

#include <utility>
#include <map>
#include <memory>

namespace rhdl {

class Entity;

class Library : public Namespace {
public:
	Library();
	virtual ~Library();

	Library &cast() override {return *this;}

	const Entity &regist(std::unique_ptr<Entity> &&e);

private:
	const Entity &regist_internal(std::unique_ptr<Entity> &&entity);
};

extern Library *defaultLib;

}

#endif // LIBRARY_H
