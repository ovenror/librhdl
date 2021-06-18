/*
 * connectionbuilder.h
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTIONSBUILDER_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTIONSBUILDER_H_

#include "../element.h"

#include <functional>
#include <memory>
#include <map>
#include <queue>
#include <set>
#include <vector>

namespace rhdl {
namespace structural {

class Connection;

namespace builder {

class Connection;
class Element;
class SimplePort;
class SimpleConnection;
class ComplexPort;
class ComplexConnection;

template <class> class TypedConnection;

class ConnectionsBuilder {
public:
	ConnectionsBuilder(const std::map<Element *, ElementIdx> &);
	virtual ~ConnectionsBuilder();

	std::vector<structural::Connection> build();

	void build(const SimpleConnection &);
	void build(const ComplexConnection &);

	void push(const Connection &);
	void add(structural::Connection);

private:
	std::set<const Connection *>
		findTopConnections() const;

	std::queue<std::reference_wrapper<const Connection>> queue_;
	const std::map<Element *, ElementIdx> &elementMap_;
	std::vector<structural::Connection> result_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTIONSBUILDER_H_ */
