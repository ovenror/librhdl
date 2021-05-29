#ifndef TIMING_H
#define TIMING_H

#include <vector>

namespace rhdl {

class Entity;
class Representation;

class Timing
{
public:
	using AbiderContainer = std::vector<const Representation *>;

	Timing(const Entity &entity);

	const Entity &entity() const {return entity_;}

private:
	void add(const Representation &r);
	bool operator==(const Timing &rhs) const ;
	const AbiderContainer &abiders() const {return abiders_;}

	const Entity &entity_;
	std::vector<const Representation *> abiders_;
};

}

#endif // TIMING_H
