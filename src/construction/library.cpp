#include "library.h"
#include "entity/einverter.h"
#include "init/init.h"

#include <cassert>

namespace rhdl {

Library *defaultLib; // = new Library();

Library::Library() : TypedCObject("entities")
{
	regist(std::make_unique<EInverter>());
	setMembers();
	init::lib_ready();
}

Library::~Library()
{}

bool Library::contains(const std::string& name) const {
	try {
		entities_.at(name);
		return true;
	}
	catch (std::out_of_range &e) {
		return false;
	}
}

const Entity &Library::regist(std::unique_ptr<Entity> &&entity)
{
	if (contains(entity -> name()))
		throw ConstructionException(Errorcode::E_ENTITY_EXISTS);

	const Entity& result = regist_internal(std::move(entity));
	setMembers();
	return result;
}

/*
Entity& Library::at(const std::string& name) {
	if (!contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_ENTITY);

	return entities_.at(name);
}
*/

const Entity& Library::at(const char *name) const
{
	if (!contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_ENTITY);

	return entities_.at(name);
}

const Entity& Library::at(const std::string &name) const
{
	if (!contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_ENTITY);

	return entities_.at(name.c_str());
}

const Entity &Library::regist_internal(std::unique_ptr<Entity> &&entity)
{
	return *entities_.add(std::move(entity));
}

void Library::setMembers_internal(const char *const *members) {
	c_.content().members = members;
}

}
