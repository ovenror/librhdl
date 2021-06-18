/*
 * structure.h
 *
 *  Created on: Jun 20, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_STRUCTURE_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_STRUCTURE_H_

#include "element.h"
#include "completestructurebuilder.h"

#include <rhdl/construction/structure.h>

#include "entity/entity.h"

#include "simulation/fast/fastsimfactory.h"

#include <memory>
#include <vector>

namespace rhdl::structural::builder {

class Structure : public Element {
	using Behavior = std::pair<std::unique_ptr<SimFactory>, bool>;

public:
	Structure(bool stateless);
	virtual ~Structure();

	ComplexPort &add(const Entity &partEntity);

	void abort();
	virtual void finalize();

	virtual bool isTheStructure() const override {return true;}
	StructureBuilder &builder() {return builder_;}


	template<class siface_class, class internal_state_class>
	void addTimedBehavior(
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> simfunc,
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> procfunc
			= [](behavioral::FastSim<siface_class, internal_state_class> &){},
		std::initializer_list<std::map<const char *, std::vector<bool> >::value_type> initial = {})
	{
		addBehavior(simfunc, procfunc, initial, true);
	}

	template<class siface_class>
	void addFunctionalBehavior(
			std::function<void(
					behavioral::FastSim<siface_class, NO_STATE> &)>
			simfunc)
	{
		addBehavior<siface_class, NO_STATE>(
				simfunc,
				[](behavioral::FastSim<siface_class, NO_STATE> &){},
				{}, false);
	}


protected:
	template<class siface_class, class internal_state_class>
	void addBehavior(
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> simfunc,
		std::function<void(behavioral::FastSim<siface_class, internal_state_class> &)> procfunc,
		std::initializer_list<std::map<const char *, std::vector<bool> >::value_type> initial,
		bool timed)
	{
		behavior_.emplace_back(
			std::make_unique<behavioral::FastSimFactory<
				siface_class, internal_state_class>>(
						simfunc, procfunc, initial), timed);
	}

	void toStream(std::ostream &os) const override;
	virtual const std::string &name() const = 0;
	bool valid() {return valid_;}
	void expect_valid();
	void doFinalize();

	bool valid_ = true;

private:
	virtual const Entity &entity() const = 0;
	void invalidate();

	CompleteStructureBuilder builder_;
	std::vector<Behavior> behavior_;
};

std::unique_ptr<Structure> makeStructure(const std::string &entityName, ::rhdl::Structure::Mode mode);

} /* namespace rhdl::structural::builder */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_STRUCTURE_H_ */
