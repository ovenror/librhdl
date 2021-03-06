#include "../tree/layer.h"

#include "../tree/wire.h"

namespace rhdl::spatial {

std::vector<const Wire *> Layer::inputs() const
{
	return inputs([](const Wire &w)
	{
		std::ignore = w;
		return true;
	});
}

std::vector<const Wire *> Layer::upperLayerInputs() const
{
	return upperLayerInputs([](const Wire &w)
	{
		std::ignore = w;
		return true;
	});
}

std::vector<const Wire *> Layer::upperLayerInputs(const Connection &connection) const
{
	return upperLayerInputs([&](const Wire &w)
	{
		return w.isConnected(connection);
	});
}

}

