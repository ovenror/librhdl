/*
 * builderport.h
 *
 *  Created on: Jul 24, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_BUILDERPORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_BUILDERPORT_H_

#include "port.h"
#include "portcontainer.h"

#include "interface/interface.h"

#include "util/dictionary/fcfsdictionary.h"

#include <string>

namespace rhdl {
namespace structural {
namespace builder {

class ExistingPort;
class NewEntityStructure;

class BuilderPort : public Port, public PortContainer {
public:
	BuilderPort(
			NewEntityStructure &structure, BuilderPort *enclosing,
			std::string name);
	virtual ~BuilderPort();

	virtual Port &operator[](const std::string &ifaceName);

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

	std::vector<const Interface *> ifaces();
	std::unique_ptr<ComplexPort> constructComplexPort(const IComposite &);

	CompositeDirection direction() const override {return direction_;}
	PortContainer *enclosing() const override {return enclosing_;}

private:
	std::unique_ptr<CompatibilityResult> compat(
			const PortContainer &peer, ConnectionPredicate p) const;

	const Interface &iface() const;

	ExistingPort &realization(Port &peer, const ConnectionPredicate &p) override;
	ExistingPort &realization(ExistingPort &peer, const ConnectionPredicate &p) override;
	ExistingPort *realization() override;

	ExistingPort &replace(std::unique_ptr<ExistingPort> &&realization);
	void replace_common(ExistingPort &realization);
	ComplexPort &replaceTop(std::unique_ptr<ComplexPort> &&realization);

	std::unique_ptr<ExistingPort> constructExistingPort(
			ExistingPort &peer, ConnectionPredicate predicate);

	void replaceEnclosedBuilder(
			BuilderPort &builder, std::unique_ptr<ExistingPort>&&newPort);

	Element &element() const override;
	Port &port() override {return *this;}
	virtual const rhdl_iface_struct *c_ptr_iface() const override {return &c_iface_;}

	BuilderPort &encloseNew(const std::string &ifaceName);
	void removeLastEnclosed(const BuilderPort &);
	void encloseDirection(CompositeDirection newDir);

	NewEntityStructure &structure_;
	BuilderPort *enclosing_;
	dictionary::FCFSDictionary<std::unique_ptr<Port>> enclosed_;
	CompositeDirection direction_;
	rhdl_iface_struct c_iface_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_BUILDERPORT_H_ */
