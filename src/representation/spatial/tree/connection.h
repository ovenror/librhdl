#ifndef TM_CONNECTION_H
#define TM_CONNECTION_H

#include "representation/netlist/netlist.h"

#include <set>
#include <map>
#include <vector>

namespace rhdl {
namespace TM {

class Wire;
class Layer;

class Connection {
public:
	using Container = std::set<Wire *>;
	using AuxiliaryIndex = std::vector<Wire *>;

	using iterator = Container::iterator;

	iterator begin() const {return all_.begin();}
	iterator end() const {return all_.end();}
	Container::size_type size() const {return all_.size();}

	const Container *layerOutputs(const Layer &layer) const;
	const AuxiliaryIndex &inverterInputs() const {return invInputs_;}
	const AuxiliaryIndex &inverterOutputs() const {return invOutputs_;}

	bool isConnected(const Wire &wire) const;

private:
	friend class Wire;

	Connection(Wire &wire);

	void accept(Wire &wire);
	void reject(Wire &wire);

	std::map<const Layer *, Container> layer_outputs_;
	Container all_;
	AuxiliaryIndex invInputs_;
	AuxiliaryIndex invOutputs_;
	bool presumeDying_;
};

}
}

#endif // CONNECTION_H
