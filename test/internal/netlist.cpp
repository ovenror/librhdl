/*
 * netlist.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: ovenror
 */

#include <gtest/gtest.h>
#include "representation/netlist/netlist.h"
#include "entity/entity.h"
#include "interface/isingle.h"
#include "util/iterable.h"

using rhdl::Entity;
using rhdl::ISingle;
using rhdl::SingleDirection;
using rhdl::netlist::Netlist;
using rhdl::netlist::VertexRef;

static Entity dummyEntity("dummy", {});

static Netlist mkNetlist(Netlist::Graph g, Netlist::InterfaceMap map = {})
{
	return Netlist(dummyEntity, g, map);
}

static Netlist::Graph transform(Netlist::Graph g) {
	Netlist nl(Entity("dummy", {}), std::move(g), {});
	return nl.graph();
}

using vVector = std::vector<VertexRef>;
static vVector dummy(20);
using vIterator = std::vector<VertexRef>::iterator;

static void addVertices(Netlist::Graph &g, vIterator v, size_t count)
{
	for (; count > 0; --count)
		*v++ = g.addVertex();
}

static void connectOneway(Netlist::Graph &g, vIterator v)
{
	g.connect(v[0], v[1]);
	g.connect(v[1], v[2]);
}

static Netlist::Graph mkVertices(vIterator v, size_t count)
{
	Netlist::Graph g;
	addVertices(g, v, count);
	return g;
}

static Netlist::Graph mkOneway(vIterator v = dummy.begin())
{
	Netlist::Graph oneway;

	addVertices(oneway, v, 3);
	connectOneway(oneway, v);

	return oneway;
}

/*
 * 0 -> 1
 * |
 * + -> 2
 */
static Netlist::Graph mkMiniVee(vIterator v = dummy.begin())
{
	Netlist::Graph miniVee;

	addVertices(miniVee, v, 3);
	miniVee.connect(v[0], v[1]);
	miniVee.connect(v[0], v[2]);

	return miniVee;
}

/*
 * 0 -> 1 -> 2
 *      |
 *      + -> 3
 */
static Netlist::Graph mkWhy(vIterator v = dummy.begin())
{
	Netlist::Graph why = mkOneway(v);

	v[3] = why.addVertex();
	why.connect(v[1], v[3]);

	return why;
}

/*
 * 0 -> 1 -> 2
 * |
 * + -> 3 -> 4
 */
static Netlist::Graph mkVee(vIterator v = dummy.begin())
{
	Netlist::Graph vee = mkOneway(v);

	addVertices(vee, v + 3, 2);
	vee.connect(v[0], v[3]);
	vee.connect(v[3], v[4]);

	return vee;
}

static Netlist::Graph mkInverter(vIterator v = dummy.begin())
{
	auto inverter = mkVertices(v, 2);
	inverter.connect(v[0], v[1]);
	return inverter;
}

const auto inverter = mkInverter();

static size_t transformedSize(Netlist::Graph g)
{
	return Iterable(transform(g).vertices()).size();
}

static bool transformsTo(Netlist::Graph g, const Netlist::Graph &ref)
{
	return transform(g).isomorphic(ref);
}

static bool isomorphic(const Netlist &nl, const Netlist::Graph &ref)
{
	return nl.graph().isomorphic(ref);
}

TEST(RemoveUnnecessaryOneways, Simple) {
	EXPECT_TRUE(transformsTo(mkOneway(), Netlist::Graph()));
}

TEST(RemoveUnnecessaryOneways, SimpleIface) {
	vVector v(3);
	const ISingle in("in", SingleDirection::IN);
	const ISingle out("out", SingleDirection::OUT);

	Netlist::Graph singleVertex;
	singleVertex.addVertex();

	auto oneway = mkOneway(v.begin());

	auto oneway0in = mkNetlist(oneway, {{&in, v[0]}});
	auto oneway0out = mkNetlist(oneway, {{&out, v[0]}});
	auto oneway1in = mkNetlist(oneway, {{&in, v[1]}});
	auto oneway1out = mkNetlist(oneway, {{&out, v[1]}});
	auto oneway2in = mkNetlist(oneway, {{&in, v[2]}});
	auto oneway2out = mkNetlist(oneway, {{&out, v[2]}});

	EXPECT_TRUE(isomorphic(oneway0in, singleVertex));
	EXPECT_TRUE(isomorphic(oneway0out, singleVertex));
	EXPECT_TRUE(isomorphic(oneway1in, oneway1in.graph()));
	EXPECT_TRUE(isomorphic(oneway1out, inverter));
	EXPECT_TRUE(isomorphic(oneway2in, singleVertex));
	EXPECT_TRUE(isomorphic(oneway2out, singleVertex));
}

TEST(RemoveUnnecessaryOneways,SimpleExtra) {
	vVector v(3);

	auto onewayExtraOut = mkOneway(v.begin());
	auto extraOut = onewayExtraOut.addVertex();
	onewayExtraOut.connect(v[0], extraOut);

	auto onewayExtraIn = mkOneway(v.begin());
	auto extraIn = onewayExtraIn.addVertex();
	onewayExtraIn.connect(extraIn, v[2]);

	EXPECT_TRUE(transformsTo(onewayExtraOut, inverter));
	EXPECT_TRUE(transformsTo(onewayExtraIn, inverter));

	const ISingle in("in", SingleDirection::IN);
	const ISingle out("out", SingleDirection::OUT);

	auto onewayExtraOut0in = mkNetlist(onewayExtraOut, {{&in, v[0]}});
	auto onewayExtraOut0out = mkNetlist(onewayExtraOut, {{&out, v[0]}});
	auto onewayExtraOut1in = mkNetlist(onewayExtraOut, {{&in, v[1]}});
	auto onewayExtraOut1out = mkNetlist(onewayExtraOut, {{&out, v[1]}});
	auto onewayExtraOut2in = mkNetlist(onewayExtraOut, {{&in, v[2]}});
	auto onewayExtraOut2out = mkNetlist(onewayExtraOut, {{&out, v[2]}});
	auto onewayExtraOutExtraIn = mkNetlist(onewayExtraOut, {{&in, extraOut}});
	auto onewayExtraOutExtraOut = mkNetlist(onewayExtraOut, {{&out, extraOut}});

	Netlist::Graph minivee = mkMiniVee();

	EXPECT_TRUE(isomorphic(onewayExtraOut0in, inverter));
	EXPECT_TRUE(isomorphic(onewayExtraOut0out, inverter));
	EXPECT_TRUE(isomorphic(onewayExtraOut1in, onewayExtraOut));
	EXPECT_TRUE(isomorphic(onewayExtraOut1out, minivee));
	EXPECT_TRUE(isomorphic(onewayExtraOut2in, onewayExtraOut));
	EXPECT_TRUE(isomorphic(onewayExtraOut2out, inverter));
	EXPECT_TRUE(isomorphic(onewayExtraOutExtraIn, inverter));
	EXPECT_TRUE(isomorphic(onewayExtraOutExtraOut, inverter));

	auto onewayExtraBoth = mkOneway(v.begin());
	extraIn = onewayExtraBoth.addVertex();
	extraOut = onewayExtraBoth.addVertex();
	onewayExtraBoth.connect(v[0], extraOut);
	onewayExtraBoth.connect(extraIn, v[2]);

	EXPECT_TRUE(transformsTo(onewayExtraBoth, onewayExtraBoth));

	/*
	 * onewayExtraBoth should never be transformed, but for the sake of
	 * completeness, test it also with interfaces in all possible places.
	 */

	for (auto v : Iterable(onewayExtraBoth.vertices())) {
		for (const auto *iface : {&in, &out}) {
			auto ifaced = mkNetlist(onewayExtraBoth, {{iface, v}});
			EXPECT_TRUE(isomorphic(ifaced, onewayExtraBoth));
		}
	}
}

TEST(RemoveUnnecessaryOneways, Y) {
	EXPECT_TRUE(transformsTo(mkWhy(), Netlist::Graph()));
}

TEST(RemoveUnnecessaryOneways, YIface) {
	vVector v(4);
	auto why = mkWhy(v.begin());

	const ISingle in("in", SingleDirection::IN);
	const ISingle out("out", SingleDirection::OUT);

	auto why0in = mkNetlist(why, {{&in, v[0]}});
	auto why0out = mkNetlist(why, {{&out, v[0]}});
	auto why1in = mkNetlist(why, {{&in, v[1]}});
	auto why1out = mkNetlist(why, {{&out, v[1]}});
	auto why2in = mkNetlist(why, {{&in, v[2]}});
	auto why2out = mkNetlist(why, {{&out, v[2]}});
	auto why3in = mkNetlist(why, {{&in, v[3]}});
	auto why3out = mkNetlist(why, {{&out, v[3]}});

	Netlist::Graph singleVertex;
	singleVertex.addVertex();
	auto oneway = mkOneway();
	auto inverter = mkInverter();

	EXPECT_TRUE(isomorphic(why0in, singleVertex));
	EXPECT_TRUE(isomorphic(why0out, singleVertex));
	EXPECT_TRUE(isomorphic(why1in, why));
	EXPECT_TRUE(isomorphic(why1out, inverter));
	EXPECT_TRUE(isomorphic(why2in, oneway));
	EXPECT_TRUE(isomorphic(why2out, singleVertex));
	EXPECT_TRUE(isomorphic(why3in, oneway));
	EXPECT_TRUE(isomorphic(why3out, singleVertex));
}


TEST(RemoveUnnecessaryOneways, YExtra) {
	vVector v(4);

	auto whyExtraOut = mkWhy(v.begin());
	auto extraOut = whyExtraOut.addVertex();
	whyExtraOut.connect(v[0], extraOut);

	EXPECT_TRUE(transformsTo(whyExtraOut, inverter));

	auto whyExtraIn0 = mkWhy(v.begin());
	auto extraIn = whyExtraIn0.addVertex();
	whyExtraIn0.connect(extraIn, v[2]);

	auto whyExtraIn1 = mkWhy(v.begin());
	extraIn = whyExtraIn1.addVertex();
	whyExtraIn1.connect(extraIn, v[3]);

	vVector resultV(3);
	auto result = mkOneway(resultV.begin());
	auto resExtraIn = result.addVertex();
	result.connect(resExtraIn, resultV[2]);

	EXPECT_TRUE(transformsTo(whyExtraIn0, result));
	EXPECT_TRUE(transformsTo(whyExtraIn1, result));
}

TEST(RemoveUnnecessaryOneways, V)
{
	EXPECT_TRUE(transformsTo(mkVee(), Netlist::Graph()));
}

TEST(RemoveUnnecessaryOneways, VExtra)
{
	vVector v(5);

	auto veeExtraOut = mkVee(v.begin());
	auto extraOut = veeExtraOut.addVertex();
	veeExtraOut.connect(v[0], extraOut);

	EXPECT_TRUE(transformsTo(veeExtraOut, inverter));

	auto veeExtraIn0 = mkVee(v.begin());
	auto extraIn = veeExtraIn0.addVertex();
	veeExtraIn0.connect(extraIn, v[2]);

	auto veeExtraIn1 = mkVee(v.begin());
	extraIn = veeExtraIn1.addVertex();
	veeExtraIn1.connect(extraIn, v[4]);

	auto result = mkOneway(v.begin());
	auto extra = result.addVertex();
	result.connect(extra, v[2]);

	EXPECT_TRUE(transformsTo(veeExtraIn0, result));
	EXPECT_TRUE(transformsTo(veeExtraIn1, result));
}
