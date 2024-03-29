#include <gtest/gtest.h>

#include "../../src/representation/spatial/tree/atomicsegment.h"
#include "representation/spatial/tree/fixoverlongwires_impl.h"
#include "representation/spatial/tree/layer.h"
#include "representation/spatial/tree/node.h"
#include "representation/spatial/tree/nodegroup.h"
#include "representation/spatial/tree/supersegment.h"
#include "representation/spatial/tree/treemodel.h"
#include "representation/spatial/tree/wire.h"
#include "representation/spatial/tree/path.h"

#include "entity/entity.h"

#include "interface/isingle.h"
#include "interface/icomposite.h"

#include "construction/library.h"

using namespace rhdl::spatial;
using rhdl::Interface;
using rhdl::ISingle;
using rhdl::IComposite;
using rhdl::netlist::Netlist;
using rhdl::netlist::VertexRef;
namespace blocks = rhdl::blocks;

class TestModel : public TreeModel {
public:
	TestModel();

	Wire *in, *out;
	Node *inv;
	Wire *c0_0;

	const ISingle *in_;
	const ISingle *out_;
	rhdl::Entity dummy_;
};

TestModel::TestModel()
	:
	  TreeModel(dummy_, nullptr, nullptr),
	  in_(new ISingle("in", Interface::Direction::IN)),
	  out_(new ISingle("out", Interface::Direction::OUT)),
	  dummy_("DUMMY", {in_, out_})
{
	const IComposite &iface = dummy_.interface();

	in = &mkBottomInterfaceWire(in_);

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
	useAsTopInterfaceWire(n1_0_0.output_, out_);

	computeSpatial();
	createSegments();
}

TEST(WireTest, wiresConnectedAt)
{
	TestModel m;

	ASSERT_EQ(m.in -> wiresConnectedAt(0).size(), 3UL);
	ASSERT_EQ(m.out -> wiresConnectedAt(m.height() - 1).size(), 3UL);
}

void testFindPaths_oneway(const Connector &from, const Connector &to, std::vector<std::vector<blocks::index_t>> &distances, Paths *paths = nullptr)
{
	Paths dummy;
	if (!paths)
		paths = &dummy;
	*paths = findPaths(Link(&from, &to));

	EXPECT_EQ(paths -> size(), distances.size());

	for (const auto &ppath : *paths) {
		const Path &path = *ppath;
		auto idists = distances.begin();

		for (; idists != distances.end(); ++ idists) {
			if (idists -> size() != path.size())
				continue;

			unsigned int i = 0;
			auto pos = 0;

			for (; i < path.size(); ++i) {
				blocks::index_t pathDist = path[i].segment().distance();

				if (path[i].reverse())
					pathDist = -pathDist;

				if (pathDist != (*idists)[i])
					break;

				if (path[i].startPos() != pos)
					break;

				pos += std::abs(pathDist);

				if (path[i].endPos() != pos)
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

void testFindPaths(const Connector &from, const Connector &to, std::vector<std::vector<blocks::index_t>> &distances, Paths *paths = nullptr)
{
	std::vector<std::vector<blocks::index_t>> rdistances;

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

	std::vector<std::vector<blocks::index_t>> pdists =
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

	std::vector<std::vector<blocks::index_t>> pdists =
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
	std::map<Link, Paths> paths = findPaths(links);
	const Path &first = *paths.at(links[0])[0];
	const Path &second = *paths.at(links[0])[1];

	EXPECT_EQ(first.length(), 8);
	EXPECT_EQ(second.length(), 12);

	GlobalRepeaterPositionToSegment map;
	auto segments = identifyCurrents(paths);

	EXPECT_FALSE(segments.empty());

	bool hasOneway = false;
	for (const auto *segment : segments) {
		if (!segment -> isOneway())
			continue;

		hasOneway = true;
		EXPECT_FALSE(&segment->firstUnique().wire() == m.c0_0);
	}

	EXPECT_TRUE(hasOneway);
}

TEST(PathEvaluation, freeLength)
{
	TestModel m;

	const Links links = {
		Link(&getIFaceConnector(*m.in), &getIFaceConnector(*m.out)),
		Link(&getIFaceConnector(*m.out), &getIFaceConnector(*m.in))
	};

	createSuperSegments(*m.in -> connection_);
	std::map<Link, Paths> paths = findPaths(links);
	identifyCurrents(paths);

	const Path &first = *paths.at(links[0])[0];
	const Path &second = *paths.at(links[0])[1];
	const Path &rsecond = *paths.at(links[1])[1];

	ASSERT_EQ (second.size(), 4UL);

	EXPECT_EQ(first.freeLength(0), 8);
	EXPECT_EQ(second.freeLength(0), 12);
	EXPECT_EQ(rsecond.freeLength(0), 12);

	EXPECT_EQ(first.freeLength(2), 6);
	EXPECT_EQ(second.freeLength(2), 10);

	EXPECT_EQ(first.freeLength(3), 5);
	EXPECT_EQ(second.freeLength(3), 9);

	EXPECT_EQ(first.freeLength(4), 4);
	EXPECT_EQ(second.freeLength(4), 8);
	EXPECT_EQ(rsecond.freeLength(4), 8);

	Segment &seg1 = second[1].segment();
	seg1.placeRepeater(1, false);

	EXPECT_EQ(seg1.nextRepeater(0, false), 1);
	EXPECT_EQ(seg1.nextRepeater(1, false), 1);
	EXPECT_EQ(seg1.nextRepeater(2, false), -1);

	EXPECT_EQ(seg1.nextRepeater(0, true), 3);
	EXPECT_EQ(seg1.nextRepeater(3, true), 3);
	EXPECT_EQ(seg1.nextRepeater(4, true), -1);

	EXPECT_EQ(second.freeLength(0), 3);
	EXPECT_EQ(second.freeLength(1), 2);
	EXPECT_EQ(second.freeLength(2), 1);

	EXPECT_EQ(second.freeLength(3), 0);
	EXPECT_EQ(second.freeLength(4), 8);
	EXPECT_EQ(second.freeLength(5), 7);
	EXPECT_EQ(second.freeLength(6), 6);

	EXPECT_EQ(rsecond.freeLength(0), 8);

	seg1.placeRepeater(1, true);

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

	Segment &seg3 = second[3].segment();
	seg3.placeRepeater(1, false);

	EXPECT_EQ(second.freeLength(5), 0);
	EXPECT_EQ(second.freeLength(6), 3);
	EXPECT_EQ(second.freeLength(9), 0);
	EXPECT_EQ(second.freeLength(10), 2);

	EXPECT_EQ(rsecond.freeLength(0), 2);
	EXPECT_EQ(rsecond.freeLength(2), 0);
	EXPECT_EQ(rsecond.freeLength(3), 3);
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
