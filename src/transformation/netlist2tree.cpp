/*
 * netlisttreeblocks.cpp
 *
 *  Created on: Jun 9, 2025
 *      Author: js
 */

#include "netlist2tree.h"
#include "representation/netlist/netlist.h"
#include "representation/spatial/tree/treemodel.h"
#include "entity/entity.h"
#include "util/debug.h"

#include <iostream>

namespace rhdl {

using netlist::Netlist;
using spatial::TreeModel;

Netlist2Tree::Netlist2Tree() : TypedTransformation("Netlist2Tree") {}

Netlist2Tree::~Netlist2Tree() {
	// TODO Auto-generated destructor stub
}

std::unique_ptr<TreeModel> Netlist2Tree::execute(
		const Netlist &netlist, const std::string &name) const
{
	std::unique_ptr<TreeModel> result;
	const auto &entity = netlist.entity();
	auto source = std::cref(netlist);
	size_t count = 0;

	while (true) {
		result = std::make_unique<TreeModel>(
				entity, &source.get(), source.get().timing(), name);
		auto &model = *result;

#if 0
		std::stringstream filename;
		filename << entity.name() << count << ".dot";
		std::ofstream dotfile(filename.str());
		dotfile << source.get().graph();
#endif

		auto vertexMap = model.createModel(source);
		model.computeSpatial();
		model.createSegments();

		auto unsynthesizableVertices = model.fixBrokenLinks(vertexMap);

		if (unsynthesizableVertices.empty())
			break;

		LOG(DEBUG) << source.get().name() << " has unsynthesizable vertices" << std::endl;

		for (auto v : unsynthesizableVertices)
			LOG(DEBUG) << "split " << v << std::endl;

		source = entity.addRepresentation(Netlist(source, unsynthesizableVertices));
		++count;
	}

	return std::move(result);
}

} /* namespace rhdl */
