/*
 * existingport.h
 *
 *  Created on: Jul 25, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORT_H_

#include "port.h"

#include "util/partitionable.h"

namespace rhdl {

class IComposite;

namespace structural {
namespace builder {

class SimplePort;
template <class> class TypedConnection;

class ExistingPort : public Port {
public:
	ExistingPort(Element &element, const Interface &iface);

	virtual ~ExistingPort();

	std::unique_ptr<CompatibilityResult> compat(
			const Port &peer, ConnectionPredicate p) const override;
	std::unique_ptr<CompatibilityResult> compat(
			const ExistingPort &peer, ConnectionPredicate p) const override;
	std::unique_ptr<CompatibilityResult> compat(
			const ComplexPort &peer, ConnectionPredicate p) const override;
	std::unique_ptr<CompatibilityResult> compat(
			const BuilderPort &peer, ConnectionPredicate p) const override;

	std::array<Port *, 2> findCompatibles(Port &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(ExistingPort &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(ComplexPort &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(BuilderPort &peer, ConnectionPredicate p) override;

	Port *findCompatible(Port &peer, ConnectionPredicate p) override;

	ExistingPort &realization(Port &peer, const ConnectionPredicate &p) override;
	ExistingPort &realization(BuilderPort &peer, const ConnectionPredicate &p) override;
	ExistingPort &realization(ExistingPort &peer, const ConnectionPredicate &p) override;
	ExistingPort *realization() override {return this;}

	CompositeDirection direction() const override;

	virtual void connectCompat(ExistingPort &peer) = 0;
	virtual void connectCompat(SimplePort &peer) {assert(0);}
	virtual void connectCompat(ComplexPort &peer) {assert(0);}

	virtual Connection &connection() const = 0;
	PortContainer *enclosing() const override {return enclosing_;}
	virtual Element &element() const override {return element_;}
	virtual const Interface &iface() const = 0;
	const std::string &name() const override;
	virtual const rhdl_iface_struct *c_ptr_iface() const override;

	void getAdoptedBy(PortContainer *p) const {enclosing_ = p;}


	void inheritHandles(BuilderPort &&);

private:
	void realizeHandles();

	Element &element_;
	mutable PortContainer *enclosing_ = nullptr;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORT_H_ */
