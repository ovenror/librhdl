/*
 * simplifiedstructural.h
 *
 *  Created on: Jun 23, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_STRUCTURE_H_
#define SRC_REPRESENTATION_STRUCTURAL_STRUCTURE_H_

#include "connection.h"

#include "representation/representationbase.h"

#include <memory>

namespace rhdl::structural {

class Structure : public RepresentationBase<Structure> {
	static_assert(ID == RHDL_STRUCTURE);
public:
	Structure(const Entity &entity,
		std::vector<const Entity *> elements,
		std::vector<Connection> connections,
		const Timing *timing, const Representation *parent);
	Structure(Structure &&) = default;

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

	const std::vector<const Entity *> &elements() const {return elements_;}
	const std::vector<Connection> &connections() const {return connections_;}

	virtual ~Structure();

private:
	const std::vector<const Entity *> elements_;
	const std::vector<Connection> connections_;
};

} /* namespace rhdl::structural */

#endif /* SRC_REPRESENTATION_STRUCTURAL_STRUCTURE_H_ */
