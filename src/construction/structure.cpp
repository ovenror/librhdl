/*
 * entityhandle.cpp
 *
 *  Created on: Apr 3, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/structure.h>
#include "library.h"

#include "representation/structural/structure.h"

#include "entity/entity.h"

#include "representation/structural/builder/existingentitystructure.h"
#include "representation/structural/builder/newentitystructure.h"

namespace rhdl {

namespace s = structural;
namespace sb = s::builder;

Structure::Structure(const std::string &name, Mode mode, Namespace *ns)
{
	if (!ns)
		ns = defaultLib;

	impl_ = sb::makeStructure(*ns, name, mode);
}

Structure::~Structure() {}

StructureConnector Structure::operator [](const std::string &iname) const
{
	return StructureConnector(port()[iname]);
}

void Structure::finalize()
{
	impl_ -> finalize();
}

void Structure::abort()
{
	impl_ -> abort();
}

structural::builder::Port &Structure::port() const
{
	return impl_ -> topPort();
}

}

