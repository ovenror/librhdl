/*
 * RepresentationContainer.h
 *
 *  Created on: Oct 11, 2024
 *      Author: js
 */

#ifndef SRC_ENTITY_REPRESENTATIONCONTAINER_H_
#define SRC_ENTITY_REPRESENTATIONCONTAINER_H_

#include <rhdl/construction/c/types.h>
#include "c_api/namespace.h"
#include "representation/representation.h"

namespace rhdl {

class RepresentationContainer: public TypedComplexCObject<
		RepresentationContainer, rhdl_namespace_struct, false>
{
	using Container = std::vector<std::unique_ptr<const Representation>>;

public:
	using iterator = Container::iterator;

	RepresentationContainer();
	virtual ~RepresentationContainer();

	RepresentationContainer &cast() override {return *this;}

	void push_back(std::unique_ptr<const Representation> &&r);

	const std::unique_ptr<const Representation> &back() const {return container_.back();}
	iterator begin() {return container_.begin();}
	iterator end() {return container_.end();}

private:
	Container container_;
};

}

#endif /* SRC_ENTITY_REPRESENTATIONCONTAINER_H_ */
