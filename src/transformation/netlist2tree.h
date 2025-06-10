/*
 * netlisttreeblocks.h
 *
 *  Created on: Jun 9, 2025
 *      Author: js
 */

#ifndef SRC_TRANSFORMATION_NETLIST2TREE_H_
#define SRC_TRANSFORMATION_NETLIST2TREE_H_

#include "typedtransformation.h"

#include <memory.h>

namespace rhdl {

namespace netlist {class Netlist;}
namespace spatial {class TreeModel;}

class Netlist2Tree : public TypedTransformation<netlist::Netlist, spatial::TreeModel>{
public:
	Netlist2Tree();
	virtual ~Netlist2Tree();

	Netlist2Tree &cast() override {return *this;}

	std::unique_ptr<spatial::TreeModel> execute(
				const netlist::Netlist &source, const std::string &result_name = "") const override;

private:

};

} /* namespace rhdl */

#endif /* SRC_TRANSFORMATION_NETLIST2TREE_H_ */
