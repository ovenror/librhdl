#include <gtest/gtest.h>

#include "representation/spatial/bgtree/fixoverlongwires_impl.h"

#include "entity/entity.h"

#include "interface/isingle.h"
#include "interface/icomposite.h"

#include "representation/spatial/bgtree/treemodel.h"
#include "representation/spatial/bgtree/layer.h"
#include "representation/spatial/bgtree/nodegroup.h"
#include "representation/spatial/bgtree/node.h"
#include "representation/spatial/bgtree/wire.h"
#include "representation/spatial/bgtree/supersegment.h"
#include "representation/spatial/bgtree/uniquesegment.h"

#include "construction/library.h"

using namespace rhdl::TM;
using rhdl::Interface;
using rhdl::ISingle;
using rhdl::IComposite;
using rhdl::blocks::Blocks;
using rhdl::netlist::Netlist;
using rhdl::netlist::VertexRef;

class TestModel : public rhdl::TreeModel {
public:
	TestModel();

	Wire *in, *out;
	Node *inv;
	Wire *c0_0;

	rhdl::Entity dummy_;
};

TestModel::TestModel()
	:
	  TreeModel(Netlist(dummy_, nullptr, nullptr), {}, {}),
	  dummy_("DUMMY", {
			  new ISingle("in", Interface::Direction::IN, false),
			  new ISingle("out", Interface::Direction::OUT, false)
	  })
{
	const IComposite &iface = dummy_.interface();

	in = &mkBottomInterfaceWire((rhdl::ISingle *) iface["in"]);

	auto &l0 = makeLayer();

	auto &c_0 = lowerCross().make();
	in->connect(c_0);

	auto &ng0_0 = l0.makeNodeGroup(nullptr, c_0);
	auto &n0_0_0 = ng0_0.makePassThrough();
	ng0_0.makePassThrough();
	auto &n0_0_2 = ng0_0.makeInverter();

	auto &c0_0 = l0.cross().make();
	this -> c0_0 = &c0_0;
	auto &c0_1 = l0.cross().make();

	n0_0_0.output_.connect(c0_0);
	n0_0_2.output_.connect(c0_1);

	auto &l1 = makeLayer();
	auto &ng1_0 = l1.makeNodeGroup(&n0_0_0, c0_0);
	auto &n1_0_0 = ng1_0.makePassThrough();
	auto &n1_0_1 = ng1_0.makeInverter();

	inv = &n1_0_1;

	auto &ng1_1 = l1.makeNodeGroup(&n0_0_2, c0_1);
	auto &n1_1_0 = ng1_1.makeInverter();
	n1_1_0.output_.connect(n1_0_0.output_);

	out = &n1_0_0.output_;
	useAsTopInterfaceWire(n1_0_0.output_, (rhdl::ISingle *) iface["out"]);

	computeSpatial();
	//assert(n0_0_0.input_anchor_.getPosition() == 1);

	createSegments();
}

TEST(WireTest, wiresConnectedAt)
{
	TestModel m;

	ASSERT_EQ(m.in -> wiresConnectedAt(0).size(), 3UL);
	ASSERT_EQ(m.out -> wiresConnectedAt(m.height() - 1).size(), 3UL);
}

void testFindPaths_oneway(const Connector &from, const Connector &to, std::vector<std::vector<Blocks::index_t>> &distances, Paths *paths = nullptr)
{
	Paths dummy;
	if (!paths)
		paths = &dummy;
	*paths = findPaths({&from, &to});

	EXPECT_EQ(paths -> size(), distances.size());

	for (const auto &ppath : *paths) {
		const Path &path = *ppath;
		auto idists = distances.begin();

		for (; idists != distances.end(); ++ idists) {
			if (idists -> size() != path.size())
				continue;

			unsigned int i = 0;

			for (; i < path.size(); ++i) {
				Blocks::index_t pathDist = path[i].first -> distance();

				if (path[i].second)
					pathDist = -pathDist;

				if (pathDist != (*idists)[i])
					break;
			}

			if (i < path.size())
				continue;

			break;
		}

		ASSERT_NE(idists, distances.end()) << "Generated path does not match one of the given distance vectors.";
		distances.erase(idists);
	}
}

void testFindPaths(const Connector &from, const Connector &to, std::vector<std::vector<Blocks::index_t>> &distances, Paths *paths = nullptr)
{
	std::vector<std::vector<Blocks::index_t>> rdistances;

	for (const auto &path : distances)
	{
		rdistances.push_back({});
		auto &reverse_path = rdistances.back();

		for (auto idist = path.rbegin(); idist < path.rend(); ++idist) {
			reverse_path.push_back(-*idist);
		}
	}

	testFindPaths_oneway(from, to, distances, paths);
	testFindPaths_oneway(to, from, rdistances);
}


TEST(FindPaths, iIn_iOut)
{
	TestModel m;

	std::vector<std::vector<Blocks::index_t>> pdists =
	{
		{
		#if 0
			1, // node input
			2, // node passthrough
			1, // node output
			1, // node input
			2, // node passthrough
		#endif
			4, // l0 node
			3  // l1 node
		},
		{
		#if 0
			2, // manifold cross
			1, // node input
			2, // node passthrough
			1, // node output
			-2, // manifold cross
			1, // node input
			2, // node passthrough
		#endif
			2, // bottom manifold cross
			4, // l0 pt node
		   -2, // l0 manifold cross
			3  // l1 pt node
		}
	};

	createSuperSegments(*m.in -> connection_);
	testFindPaths(getIFaceConnector(*m.in), getIFaceConnector(*m.out), pdists);
}

TEST(FindPaths, iIn_Inv)
{
	TestModel m;

	std::vector<std::vector<Blocks::index_t>> pdists =
	{
		{
		#if 0
			1, // node input
			2, // node passthrough
			1, // node output
			2, // manifold cross
			1, // node input
		#endif
			4, // l0 pt node
			2, // l0 manifold cross
			1, // node input
		},
		{
		#if 0
			2, // manifold cross
			1, // node input
			2, // node passthrough
			1, // node output
			1, // node input
		#endif
			2, // bottom manifold cross
			4, // l0 pt node
			1, // node input
		}
	};

	createSuperSegments(*m.in -> connection_);
	testFindPaths(getIFaceConnector(*m.in), getInputConnector(*m.inv), pdists);
}

TEST(PathEvaluation, shortPaths)
{
	TestModel m;

	const Links links = {
		Link(&getIFaceConnector(*m.in), &getIFaceConnector(*m.out)),
		Link(&getIFaceConnector(*m.in), &getInputConnector(*m.inv))
	};

	createSuperSegments(*m.in -> connection_);
	std::map<Link, Paths> paths = findPaths({{}, links});
	const Path &first = *paths.at(links[0])[0];
	const Path &second = *paths.at(links[0])[1];

	EXPECT_EQ(length(first), 8);
	EXPECT_EQ(length(second), 12);

	SegmentToPositionIndex map;
	auto eligible = identifyEligibleCurrents(paths);

	EXPECT_FALSE(eligible.empty());

	for (const auto &kv : eligible) {
		const Segment *segment = kv.first;
		EXPECT_FALSE(&segment->firstUnique().wire() == m.c0_0);
	}
}

TEST(PathEvaluation, freeLength)
{
	TestModel m;

	const Links links = {
		Link(&getIFaceConnector(*m.in), &getIFaceConnector(*m.out)),
		Link(&getIFaceConnector(*m.out), &getIFaceConnector(*m.in))
	};

	createSuperSegments(*m.in -> connection_);
	std::map<Link, Paths> paths = findPaths({{}, links});

	const Path &first = *paths.at(links[0])[0];
	const Path &second = *paths.at(links[0])[1];
	const Path &rsecond = *paths.at(links[1])[1];

	ASSERT_EQ (second.size(), 4UL);

	EXPECT_EQ(freeLength(first, 0), 8);
	EXPECT_EQ(freeLength(second, 0), 12);
	EXPECT_EQ(freeLength(rsecond, 0), 12);

	EXPECT_EQ(freeLength(first, 2), 6);
	EXPECT_EQ(freeLength(second, 2), 10);

	EXPECT_EQ(freeLength(first, 3), 5);
	EXPECT_EQ(freeLength(second, 3), 9);

	EXPECT_EQ(freeLength(first, 4), 4);
	EXPECT_EQ(freeLength(second, 4), 8);
	EXPECT_EQ(freeLength(rsecond, 4), 8);


	Blocks dummyTarget(m.dummy_, nullptr, nullptr);
	dummyTarget.resize({4, m.width(), m.height()});

	const Segment &seg1 = *second[1].first;
	seg1.placeRepeater(1, false, dummyTarget);

	EXPECT_EQ(seg1.nextRepeater(0, false), 1);
	EXPECT_EQ(seg1.nextRepeater(1, false), 1);
	EXPECT_EQ(seg1.nextRepeater(2, false), -1);

	EXPECT_EQ(seg1.nextRepeater(0, true), 3);
	EXPECT_EQ(seg1.nextRepeater(3, true), 3);
	EXPECT_EQ(seg1.nextRepeater(4, true), -1);

	EXPECT_EQ(freeLength(second, 0), 3);
	EXPECT_EQ(freeLength(second, 1), 2);
	EXPECT_EQ(freeLength(second, 2), 1);

	EXPECT_EQ(freeLength(second, 3), 0);
	EXPECT_EQ(freeLength(second, 4), 8);
	EXPECT_EQ(freeLength(second, 5), 7);
	EXPECT_EQ(freeLength(second, 6), 6);

	EXPECT_EQ(freeLength(rsecond, 0), 8);

	seg1.placeRepeater(1, true, dummyTarget);

	EXPECT_EQ(seg1.nextRepeater(0, false), 1);
	EXPECT_EQ(seg1.nextRepeater(1, false), 1);
	EXPECT_EQ(seg1.nextRepeater(2, false), 3);
	EXPECT_EQ(seg1.nextRepeater(3, false), 3);
	EXPECT_EQ(seg1.nextRepeater(4, false), -1);

	EXPECT_EQ(seg1.nextRepeater(0, true), 1);
	EXPECT_EQ(seg1.nextRepeater(1, true), 1);
	EXPECT_EQ(seg1.nextRepeater(2, true), 3);
	EXPECT_EQ(seg1.nextRepeater(3, true), 3);
	EXPECT_EQ(seg1.nextRepeater(4, true), -1);

	const Segment &seg3 = *second[3].first;
	seg3.placeRepeater(1, false, dummyTarget);

	EXPECT_EQ(freeLength(second, 5), 0);
	EXPECT_EQ(freeLength(second, 6), 3);
	EXPECT_EQ(freeLength(second, 9), 0);
	EXPECT_EQ(freeLength(second, 10), 2);

	EXPECT_EQ(freeLength(rsecond, 0), 2);
	EXPECT_EQ(freeLength(rsecond, 2), 0);
	EXPECT_EQ(freeLength(rsecond, 3), 3);
}

TEST(PathEvaluation, rateSplitPath)
{
	EXPECT_EQ(rate(0), SplitPathRating(0, 15));
	EXPECT_EQ(rate(15), SplitPathRating(0, 0));
	EXPECT_EQ(rate(16), SplitPathRating(1, 15));
}

TEST(PathEvaluation, ratePath)
{
	EXPECT_EQ(rate(30,16), PositionRating({SplitPathRating(0, 2), SplitPathRating(1, 15)}));
	EXPECT_EQ(rate(30,15), PositionRating({SplitPathRating(0, 0), SplitPathRating(0, 1)}));
	EXPECT_EQ(rate(31,15), PositionRating({SplitPathRating(0, 0), SplitPathRating(0, 0)}));
	EXPECT_EQ(rate(31,16), PositionRating({SplitPathRating(0, 1), SplitPathRating(1, 15)}));
}
