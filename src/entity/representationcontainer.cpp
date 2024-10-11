/*
 * RepresentationContainer.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: js
 */

#include <entity/representationcontainer.h>

namespace rhdl {

RepresentationContainer::RepresentationContainer()
			: CObjectImpl("representations") {}

RepresentationContainer::~RepresentationContainer() {}

void rhdl::RepresentationContainer::push_back(
		std::unique_ptr<Representation> &&r)
{
	add(r.get());
	container_.push_back(std::move(r));
}

}
