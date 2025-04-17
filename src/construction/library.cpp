#include "library.h"
#include "entity/einverter.h"
#include "init/init.h"

#include <cassert>

namespace rhdl {

Library *defaultLib; // = new Library();

Library::Library() : Namespace("entities")
{
	regist(std::make_unique<EInverter>());
	init::lib_ready();
}

Library::~Library()
{}

const Entity &Library::regist(std::unique_ptr<Entity> &&entity)
{
	const Entity& result = regist_internal(std::move(entity));
	return result;
}

const Entity &Library::regist_internal(std::unique_ptr<Entity> &&entity)
{
	return add(std::move(entity));
}

}
