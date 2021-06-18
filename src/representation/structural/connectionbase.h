/*
 * connectionbase.h
 *
 *  Created on: Jun 16, 2021
 *      Author: ovenror
 */

#ifndef SRC_TRANSFORMATION_CONNECTIONBASE_H_
#define SRC_TRANSFORMATION_CONNECTIONBASE_H_

#include "interface/direction.h"

#include "util/iterable.h"
#include "util/dereference_iterator.h"

#include <boost/iterator/transform_iterator.hpp>

#include <array>
#include <ostream>
#include <utility>

namespace rhdl::structural {

template <class CONTAINER>
class ConnectionBase {
public:
	using iterator = typename CONTAINER::iterator;
	using const_iterator = typename CONTAINER::const_iterator;
	using PortsIterable = Iterable<iterator>;
	using const_PortsIterable = Iterable<const_iterator>;

	ConnectionBase() {}

	template <class... Args>
	ConnectionBase(Args&&... args) : container_(std::forward<Args>(args)...) {}

	ConnectionBase(CONTAINER container) : container_(std::move(container)) {}

	auto cbegin() const {return container_.cbegin();}
	auto cend() const {return container_.cend();}
	auto begin() const {return container_.begin();}
	auto end() const {return container_.end();}
	auto begin() {return container_.begin();}
	auto end() {return container_.end();}

	bool empty();
	bool valid();

	void toStream(std::ostream &os) const;

protected:
	CONTAINER container_;

private:
	template <class, class, class> friend class Partitionable;

	void sideToStream(std::ostream &os, SingleDirection dir) const;
};

template<class CONTAINER>
inline void ConnectionBase<CONTAINER>::toStream(std::ostream &os) const
{
	sideToStream(os, SingleDirection::OUT);
	os << " -> ";
	sideToStream(os, SingleDirection::IN);
}

template<class CONTAINER>
inline bool ConnectionBase<CONTAINER>::empty()
{
	return container_.empty();
}

template<class CONTAINER>
inline bool ConnectionBase<CONTAINER>::valid()
{
	return !empty();
}

template<class CONTAINER>
inline void ConnectionBase<CONTAINER>::sideToStream(
		std::ostream &os, SingleDirection dir) const
{
	os << "(";

	for (const auto &port : container_) {
		if (port.iface().preferredDirection() == dir)
			os << port << "; ";
	}

	os << ")";
}

} /* namespace rhdl::structural */

#endif /* SRC_TRANSFORMATION_CONNECTIONBASE_H_ */
