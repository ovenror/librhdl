#ifndef FUNCTIONALBEHAVIOR_H
#define FUNCTIONALBEHAVIOR_H

#include "representation/representationbase.h"

namespace rhdl {

class FunctionalBehavior : public RepresentationBase<FunctionalBehavior>
{
public:
	FunctionalBehavior(const Entity &entity, const Representation *parent);

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const;
};

}
#endif // FUNCTIONALBEHAVIOR_H
