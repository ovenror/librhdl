/*
 * handle.cpp
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#include "handle.h"

namespace rhdl {

Handle::Handle(Context& context)
	: context_(context), c_(*this)
{}

Handle::~Handle() {}

void Handle::pt() const
{
	throw ConstructionException(Errorcode::E_ILLEGAL_PASSTHROUGH);
}

}

