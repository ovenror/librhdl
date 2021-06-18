/*
 * connection.h
 *
 *  Created on: Jun 14, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTION_H_

#include "existingport.h"

#include "../connection.h"

#include "util/catiterator.h"
#include "util/iterable.h"

#include <cassert>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>

namespace rhdl::structural::builder {

class ComplexConnection;
class ConnectionsBuilder;

class Connection {
public:
	Connection() {}
	virtual ~Connection() {}

	bool connected(ExistingPort &p) {return &p.connection() == this;}

	void addParent(ComplexConnection &);

	virtual void build(ConnectionsBuilder &) const = 0;

	virtual void adoptRelativesFrom(Connection &);
	virtual void getRelativesAdoptedBy(ComplexConnection &) {assert (0);}

	void replaceParent(ComplexConnection &, ComplexConnection &);

protected:
	Side findSide(ExistingPort &, SingleDirection dir);

	template <class CONNECTION>
	static void copyRelatives(const std::set<CONNECTION *> &from, std::set<CONNECTION *> &to);

	std::set<ComplexConnection *> parents_;
};

template<class CONNECTION>
inline void Connection::copyRelatives(
		const std::set<CONNECTION*> &from, std::set<CONNECTION*> &to)
{
	std::copy(from.begin(), from.end(), std::inserter(to, to.begin()));
}


} /* namespace rhdl::structural::builder */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_CONNECTION_H_ */
