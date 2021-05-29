#include "library.h"

#include "entity/einverter.h"

#include "representation/structural/structural.h"
#include "representation/structural/temporarynet.h"

#include "init/init.h"

#include <cassert>

namespace rhdl {

Library defaultLib;

Library::Library() : c_(*this)
{
	regist(std::make_unique<EInverter>());
	c_.content().entities = entities_.c_strings();
	init::lib_ready();
}

Library::~Library()
{
}

bool Library::contains(const std::string& name) {
	return (entities_.find(name) != entities_.end());
}

Entity &Library::regist(std::unique_ptr<Entity> &&entity)
{
	if (contains(entity -> name()))
		throw ConstructionException(Errorcode::E_ENTITY_EXISTS);

	Entity& result = regist_internal(std::move(entity));
	c_.content().entities = entities_.c_strings();
	return result;
}

Entity& Library::at(const std::string& name) {
	if (!contains(name))
		throw ConstructionException(Errorcode::E_NO_SUCH_ENTITY);

	return *entities_.at(name);
}

PartHandle Library::New(const std::string &name)
{
	return PartHandle(&at(name));
}

Entity &Library::regist_internal(std::unique_ptr<Entity> &&entity)
{
	const auto [iter, inserted] = entities_.insert(std::move(entity));
	assert (inserted);

	return **iter;
}

}
