#ifndef LAYER_H
#define LAYER_H

#include "representation/blocks/blocks.h"

#include <vector>
#include <functional>
#include <memory>

namespace rhdl {

class TreeModel;

namespace TM {

class Wires;
class Wire;
class Connection;
class Container;

class Layer
{
public:
	virtual const Container &asContainer() const = 0;

	virtual const Layer *above() const = 0;
	virtual const Layer *below() const = 0;

	virtual const Wires &cross() const = 0;
	virtual Wires &cross() = 0;

	virtual const Wires &crossBelow() const = 0;
	virtual Wires &crossBelow() = 0;

	virtual std::vector<const Wire *> inputs(const std::function<bool (const Wire &)> &predicate) const = 0;
	std::vector<const Wire *> inputs() const;

	std::vector<const Wire *> upperLayerInputs() const;
	std::vector<const Wire *> upperLayerInputs(const Connection &connection) const;
	virtual std::vector<const Wire *> upperLayerInputs(const std::function<bool(const Wire &)> &predicate) const = 0;
};

}}

#endif // LAYER_H
