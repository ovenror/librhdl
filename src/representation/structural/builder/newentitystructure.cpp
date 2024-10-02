/*
 * newentitystructure.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: ovenror
 */

#include "newentitystructure.h"
#include "builderport.h"
#include "complexport.h"

#include "construction/library.h"
#include "entity/entity.h"

#include <memory>

namespace rhdl {
namespace structural {
namespace builder {

NewEntityStructure::NewEntityStructure(std::string entityName, bool stateless)
	: Structure(stateless), entityName_(entityName)
{
	if (defaultLib -> contains(entityName)) {
		valid_ = false;
		throw ConstructionException(Errorcode::E_ENTITY_EXISTS);
	}

	auto top = std::make_unique<BuilderPort>(*this, nullptr, 0, "");
	topBuilder = top.get();
	top_ = std::move(top);
}

NewEntityStructure::~NewEntityStructure()
{
	if (valid())
		doFinalize();
}

void NewEntityStructure::finalize()
{
	expect_valid();
	doFinalize();
}

void NewEntityStructure::doFinalize()
{
	assert (topBuilder);
	auto ifaces = topBuilder -> ifaces();

	if (ifaces.empty()) {
		abort();
		throw ConstructionException(Errorcode::E_EMPTY_INTERFACE);
	}

	auto entity = std::make_unique<Entity>(entityName_, std::move(ifaces), builder().stateless());

	if (defaultLib -> contains(entity -> name())) {
		abort();
		throw ConstructionException(Errorcode::E_ENTITY_EXISTS);
	}

	defaultLib -> regist(std::move(entity));
	Structure::doFinalize();
}

const Entity& NewEntityStructure::entity() const
{
	assert (defaultLib -> contains(entityName_));
	return defaultLib -> at(entityName_);
}

void NewEntityStructure::replaceTopBuilder(std::unique_ptr<ComplexPort> &&e)
{
	top_ = std::move(e);
	topBuilder = nullptr;
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
