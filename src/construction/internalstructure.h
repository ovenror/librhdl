/*
 * extendedstructure.h
 *
 *  Created on: Jul 28, 2021
 *      Author: ovenror
 */

#ifndef SRC_CONSTRUCTION_INTERNALSTRUCTURE_H_
#define SRC_CONSTRUCTION_INTERNALSTRUCTURE_H_

#include <rhdl/construction/structure.h>
#include "construction/buildsim.h"
#include "representation/structural/builder/structure.h"
#include <functional>

namespace rhdl {

class InternalStructure : public Structure {
public:
	InternalStructure(const std::string &name, Mode mode = CREATE_STATELESS);
	virtual ~InternalStructure() {};

	template<class siface_class, class internal_state_class>
	void addTimedBehavior(
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> simfunc,
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> procfunc
			= [](behavioral::FastSim<siface_class, internal_state_class> &){},
		std::initializer_list<std::map<const char *, std::vector<bool> >::value_type> initial = {})
	{
		impl_ -> addTimedBehavior(simfunc, procfunc, initial);
	}

	template<class siface_class>
	void addFunctionalBehavior(
			std::function<void(
					behavioral::FastSim<siface_class, NO_STATE> &)>
			simfunc)
	{
		impl_ -> addFunctionalBehavior(simfunc);
	}
};

} /* namespace rhdl */

#endif /* SRC_CONSTRUCTION_INTERNALSTRUCTURE_H_ */
