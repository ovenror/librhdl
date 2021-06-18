/*
 * complexinterfaceindex.h
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXPORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXPORT_H_

#include "portcontainer.h"
#include "typedexistingport.h"

#include "util/polycontainer/poly.h"

#include <set>

namespace rhdl {

class Connectible;
class IComposite;

namespace structural::builder {

class ComplexConnection;

class ComplexPort : public TypedExistingPort<ComplexPort>, public PortContainer {
public:
	using Enclosed = pc::Poly<std::set<std::unique_ptr<ExistingPort>, PortContainer::Less>, Port>;

	ComplexPort(
			Element &element, const IComposite &iface, Enclosed &&enclosed);

	virtual ~ComplexPort();

	Port &operator[](const std::string &ifaceName) override;

	std::unique_ptr<CompatibilityResult> compat(
			const Port &peer, ConnectionPredicate p) const override;
	std::unique_ptr<CompatibilityResult> compat(
			const BuilderPort &peer, ConnectionPredicate p) const override;

	std::array<Port *, 2> findCompatibles(Port &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(ExistingPort &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(ComplexPort &peer, ConnectionPredicate p) override;
	std::array<Port *, 2> findCompatibles(BuilderPort &peer, ConnectionPredicate p) override;

	Port *findCompatible(Port &peer, ConnectionPredicate p) override;

	void autoconsolidate() {}

	virtual void connectCompat(ExistingPort &peer) override;
	virtual void connectCompat(ComplexPort &peer) override;

	const Interface &iface() const override;

private:
	const Enclosed &enclosed() const override {return enclosed_;}
	Port &port() override {return *this;}
	void adoptEnclosed();
	void establishConnectionRelations(ComplexConnection &);

	template <bool (*f)(ExistingPort &, ExistingPort &)>
	bool componentWise(const ComplexPort &) const;

	static bool componentWise(
			const std::vector<const ComplexPort*> &,
			const std::function<bool(const std::vector<const ExistingPort *>)> &);

	ExistingPort &get(const Interface &) const;

	const IComposite &iface_;
	Enclosed enclosed_;
};

} /* namespace structural::builder */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXPORT_H_ */
