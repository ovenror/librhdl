/*
 * complexport.h
 *
 *  Created on: Jul 25, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_

#include "util/polycontainer/abstractpoly.h"
#include <memory>
#include <set>

namespace rhdl {

class CompatibilityResult;
class ConnectionPredicate;

namespace structural {
namespace builder {

namespace pc = polycontainer;

class Port;
class ExistingPort;
class ComplexPort;
class CBuilderPortContainerOps;

class PortContainer {
public:
	PortContainer();
	virtual ~PortContainer();

	static void tryConsolidate(PortContainer &from, PortContainer &to);
	virtual void tryConsolidate(ComplexPort &to) {}

	virtual Port &port() = 0;
	const Port &port() const;

protected:
	struct Less {
		using is_transparent = void;

		bool operator()(const Port &lhs, const Port &rhs) const;
		bool operator()(const Port &lhs, const std::string &rhs) const;
		bool operator()(const std::string &lhs, const Port &rhs) const;

		bool operator()(
				const std::unique_ptr<Port> &lhs,
				const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(*lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<Port> &lhs, const Port &rhs) const
		{
			return (*this)(*lhs, rhs);
		}

		bool operator()(
				const Port &lhs, const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<Port> &lhs, const std::string &rhs) const
		{
			return (*this)(*lhs, rhs);
		}

		bool operator()(
				const std::string &lhs, const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const Port &rhs) const;
		bool operator()(
				const Port &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const std::string &rhs) const;
		bool operator()(
				const std::string &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
	};

	std::array<Port *, 2> findCompatibles(Port &peer, ConnectionPredicate p);
	Port *findCompatible(Port &peer, ConnectionPredicate p);

	void adopt(const ExistingPort &p);

private:
	friend class CBuilderPortContainerOps;

	virtual const pc::AbstractPoly<Port> &enclosed() const = 0;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_ */
