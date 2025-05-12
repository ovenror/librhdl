/*
 * complexinterfaceindex.h
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXPORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXPORT_H_

#include <representation/structural/builder/existingportbase.h>
#include "portcontainer.h"
#include "util/dictionary/fcfsdictionary.h"

#include <set>

namespace rhdl {

class Connectible;
class IComposite;

namespace structural::builder {

class ComplexConnection;

class ComplexPort : public ExistingPortBase<ComplexPort>, public PortContainer {
public:
	using Enclosed = dictionary::FCFSDictionary<std::unique_ptr<ExistingPort>>;

	ComplexPort(
			Element &element, const IComposite &iface, Enclosed &&enclosed,
			const std::string *name = nullptr);

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

protected:
	ComplexPort &cast() override {return *this;}

private:
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
