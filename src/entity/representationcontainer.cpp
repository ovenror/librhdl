/*
 * RepresentationContainer.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: js
 */

#include "representationcontainer.h"
#include "entity.h"

namespace rhdl {

RepresentationContainer::RepresentationContainer()
			: TypedCObject("representations") {}

RepresentationContainer::~RepresentationContainer() {}

void rhdl::RepresentationContainer::push_back(
		std::unique_ptr<const Representation> &&r)
{
	add(r.get());
	container_.push_back(std::move(r));
}

}
