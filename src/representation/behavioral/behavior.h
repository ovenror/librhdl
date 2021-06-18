#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "representation/representation.h"

namespace rhdl::behavioral {

class Behavior : public Representation
{
public:
	Behavior(const Entity &entity);
};

}

#endif // BEHAVIOR_H
