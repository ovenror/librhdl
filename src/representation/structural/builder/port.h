/*
 * interfaceindex.h
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORT_H_

#include "construction/connectionpredicate.h"
#include "c_api/wrapper.h"
#include "interface/compositedirection.h"

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

namespace rhdl {

class Interface;
class CompatibilityResult;
class ConstructionException;
class PortHandle;

namespace structural::builder {

class Connection;
class Side;
class Port;
class ExistingPort;
class BuilderPort;
class PortContainer;
class ComplexPort;
class Element;
class StructureBuilder;

class Port {
public:
	Port();
	virtual ~Port();

	virtual Port &operator[](const std::string &ifaceName);

	void registerHandle(PortHandle *handle);
	void removeHandle(PortHandle *handle);
	void invalidateHandles();
	void realizeHandles(ExistingPort &realization);

	bool isExternal();

	void connectTo(Port &peer);
	static void connect(Port &, Port &, bool directional = true);

	static bool compatible(const Port &from, const Port &to);
	bool compatible(const Port &peer, ConnectionPredicate p) const;
	static bool interchangeable(const Port &lhs, const Port &rhs);

	static std::unique_ptr<CompatibilityResult> compat(
			const Port &from, const Port &to);
	std::unique_ptr<CompatibilityResult> compatTo(
			const Port &target) const;
	virtual std::unique_ptr<CompatibilityResult> compat(
			const Port &peer, ConnectionPredicate p) const = 0;
	virtual std::unique_ptr<CompatibilityResult> compat(
			const ExistingPort &peer, ConnectionPredicate p) const = 0;
	virtual std::unique_ptr<CompatibilityResult> compat(
			const BuilderPort &peer, ConnectionPredicate p) const = 0;
	virtual std::unique_ptr<CompatibilityResult> compat(
			const ComplexPort &peer, ConnectionPredicate p) const = 0;

	static std::array<Port *, 2> findCompatibles(
			Port &from, Port &to, const ConnectionPredicate &p);

	virtual std::array<Port *, 2> findCompatibles(Port &peer, ConnectionPredicate p) = 0;
	virtual std::array<Port *, 2> findCompatibles(ExistingPort &peer, ConnectionPredicate p) = 0;
	virtual std::array<Port *, 2> findCompatibles(ComplexPort &peer, ConnectionPredicate p) = 0;
	virtual std::array<Port *, 2> findCompatibles(BuilderPort &peer, ConnectionPredicate p) = 0;

	virtual Port *findCompatible(Port &peer, ConnectionPredicate p) = 0;

	std::array<ExistingPort *, 2> realize(Port &from, Port &to);
	virtual ExistingPort &realization(Port &peer, const ConnectionPredicate &p) = 0;
	virtual ExistingPort &realization(BuilderPort &peer, const ConnectionPredicate &p);
	virtual ExistingPort &realization(ExistingPort &peer, const ConnectionPredicate &p) = 0;
	virtual ExistingPort *realization() = 0;

	virtual Element &element() const = 0;
	virtual PortContainer *enclosing() const = 0;
	virtual const std::string &name() const = 0;
	virtual CompositeDirection direction() const = 0;
	virtual const rhdl_iface_struct *c_ptr_iface() const = 0;

	bool operator<(const Port &p) const;

protected:
	void qnameToStream(std::ostream &) const;
	static ConnectionPredicate predicate(
			const Port &from, const Port &to, bool directional = true);
	void inheritHandles(Port &&dying);
	const auto handles() const {return handles_;}

private:
	static void connectCompat(
			Port &from, Port &to, const ConnectionPredicate &p);
	friend std::ostream &operator<<(std::ostream &os, const Port &p);

	std::unordered_set<PortHandle *> handles_;
};

std::ostream &operator<<(std::ostream &os, const Port &p);

} /* namespace structural::builder */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORT_H_ */
