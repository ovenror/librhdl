#include "../tree/connection.h"

#include <cassert>
#include "../tree/layer.h"
#include "../tree/node.h"
#include "../tree/nodegroup.h"
#include "../tree/wire.h"

namespace rhdl {
namespace spatial {

Connection::Connection(Wire &wire)
	: presumeDying_(false)
{
	assert ((unsigned long) &wire > 10);

	all_.insert(&wire);

	Node *n = wire.getNode();

	if (!n)
		return;

	bool inverter = n -> invert_;

	if (wire.isInputOf(n) && inverter) {
		invInputs_.push_back(&wire);
		return;
	}

	if (!wire.isOutputOf(n))
		return;

	if (inverter)
		invOutputs_.push_back(&wire);

	layer_outputs_[n -> getLayer()].insert(&wire);
}

void Connection::accept(Wire &wire)
{
	assert (!presumeDying_);
	assert(!isConnected(wire));
	assert(wire.connection_ -> isConnected(wire));

	Connection &w_conn = *wire.connection_;
	Container &w_all = w_conn.all_;
	AuxiliaryIndex &w_in = w_conn.invInputs_;
	AuxiliaryIndex &w_out = w_conn.invOutputs_;

	for (Wire *pwire : w_all) {
		assert ((unsigned long) pwire > 10);
	}

	std::move(w_all.begin(), w_all.end(), std::inserter(all_, all_.end()));
	std::move(w_in.begin(), w_in.end(), std::back_inserter(invInputs_));
	std::move(w_out.begin(), w_out.end(), std::back_inserter(invOutputs_));

	for (auto &kv : w_conn.layer_outputs_) {
		const Layer *layer = kv.first;
		Container &container = kv.second;

		std::move(container.begin(), container.end(), std::inserter(layer_outputs_[layer], layer_outputs_[layer].end()));
	}
}

void Connection::reject(Wire &wire)
{
	if (wire.getNode())
		presumeDying_ = true;

	all_.erase(&wire);

	if (wire.getLayer()) {
		try {
			layer_outputs_[wire.getLayer()].erase(&wire);
		}
		catch (...) {}
	}
}

const Connection::Container *Connection::layerOutputs(const Layer &layer) const
{
	auto iter = layer_outputs_.find(&layer);

	if (iter == layer_outputs_.end())
		return nullptr;

	return &(iter -> second);
}

bool Connection::isConnected(const Wire &wire) const
{
	bool result = all_.find(const_cast<Wire *>(&wire)) != all_.end();
	assert (result == (wire.connection_.get() == this));
	return result;
}


}
}
