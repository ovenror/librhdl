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

NewEntityStructure::NewEntityStructure(
		Namespace &ns, std::string entityName, bool stateless)
		: Structure(entityName, stateless), entityName_(entityName), ns_(ns)
{
	if (ns.contains(entityName)) {
		valid_ = false;
		throw ConstructionException(Errorcode::E_MEMBER_EXISTS);
	}

	auto top = std::make_unique<BuilderPort>(*this, nullptr, entityName);
	topBuilder_ = top.get();
	top_ = std::move(top);
	init_c(*topBuilder_);
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
	assert (topBuilder_);
	auto ifaces = topBuilder_ -> ifaces();

	if (ifaces.empty()) {
		abort();
		throw ConstructionException(Errorcode::E_EMPTY_INTERFACE);
	}

	if (ns_.contains(entityName_)) {
		abort();
		throw ConstructionException(Errorcode::E_MEMBER_EXISTS);
	}

	entity_ = std::make_unique<Entity>(entityName_, std::move(ifaces), builder().stateless());
	Structure::doFinalize();
	ns_.add(std::move(entity_));
}

const Entity& NewEntityStructure::entity() const
{
	return *entity_;
}

void NewEntityStructure::replaceTopBuilder(std::unique_ptr<ComplexPort> &&e)
{
	top_ = std::move(e);
	topBuilder_ = nullptr;
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
