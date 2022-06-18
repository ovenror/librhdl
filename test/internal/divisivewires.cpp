#include "representation/spatial/tree/divisivewires.h"
#include "representation/spatial/tree/horizontal.h"
#include "representation/spatial/tree/layer.h"
#include "representation/spatial/tree/singlewire.h"
#include "representation/spatial/tree/wire.h"

#include <gtest/gtest.h>

using namespace rhdl;
using namespace rhdl::spatial;

class TestLayer : public Container, Layer {
public:
	TestLayer(unsigned int index) : Container(index) {}

	const Layer *getLayer() const override {return this;}

	const Container &asContainer() const override {return *this;}

	virtual blocks::Blocks::index_t xpos() const {return 0;}
	virtual blocks::Blocks::index_t ypos() const {return 0;}

	virtual const Layer *above() const override {assert (0);}
	virtual const Layer *below() const override {assert (0);}

	virtual const Wires &cross() const override {assert (0);}
	virtual Wires &cross() override {assert (0);}

	virtual const Wires &crossBelow() const override {assert (0);}
	virtual Wires &crossBelow() override {assert (0);}

	virtual std::vector<const Wire *> inputs(const std::function<bool (const Wire &)> &predicate) const override {
		std::ignore = predicate;
		assert (0);
	}

	virtual std::vector<const Wire *> upperLayerInputs(const std::function<bool(const Wire &)> &predicate) const override {
		std::ignore = predicate;
		return {};
	}
};

TEST(DivisiveWiresTest, ordering)
{
	constexpr unsigned int N = 11;

	TestLayer upperLayer(1);
	TestLayer lowerLayer(0);
	DivisiveWires cross(lowerLayer);

	std::array<SingleWire *, N> above;
	std::array<Wire *, N> ws;
	std::array<SingleWire *, N> below;

	for (Wire *&w : ws)
		w = &cross.make();

	for (unsigned i=0; i < N; ++i) {
		above[i] = new SingleWire(upperLayer, true);
		below[i] = new SingleWire(lowerLayer, true);

		above[i] -> addCrosser(cross);
		below[i] -> addCrosser(cross);

		above[i] -> connect(*ws[i]);
		below[i] -> connect(*ws[(i+7) % N]);
	}

	for (unsigned i=0; i < N-1; ++i) {
		above[i] -> setPosition(i*2);
		below[i] -> setPosition(i*2);
	}

	above[N-1] -> setPosition(N*2-2);
	EXPECT_FALSE(cross.mayBeCrossed(*below[N-1], N*2-2));

	cross.computePositions();

	for (unsigned i=0; i < N-1; ++i) {
		Wire &wUpper = *ws[i];
		Wire &wLower = *ws[(i+7) % N];

		assert (wUpper.isConnected(*above[i]));
		assert (wLower.isConnected(*below[i]));

		EXPECT_TRUE(wLower.relativePosition_ < wUpper.relativePosition_);
	}

	EXPECT_TRUE(true);
}
