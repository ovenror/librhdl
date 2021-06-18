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

#include "util/polycontainer/poly.h"

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
			size_t index, std::string name);
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
	const pc::Poly<std::set<std::unique_ptr<Port>, Less>> &enclosed() const override {return enclosed_;}
	PortContainer *enclosing() const override {return enclosing_;}

private:
	std::unique_ptr<CompatibilityResult> compat(
			const PortContainer &peer, ConnectionPredicate p) const;

	virtual const std::string &name() const override {return name_;}
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
	virtual const rhdl_iface_struct *c_ptr_iface() const override;

	BuilderPort &encloseNew(const std::string &ifaceName = Interface::anon_name);
	void removeLastEnclosed(const BuilderPort &);
	void encloseDirection(CompositeDirection newDir);

	void assertLenghts() const;

	NewEntityStructure &structure_;
	BuilderPort *enclosing_;
	const size_t orderIndex_;
	std::string name_;
	pc::Poly<std::set<std::unique_ptr<Port>, Less>> enclosed_;
	std::vector<Port *> enclosedOrder_;
	CompositeDirection direction_;
	std::vector<const char *> c_strings_ = {nullptr};

public:
	using C_Struct = rhdl_iface_struct;

protected:
	friend class Wrapper<BuilderPort>;
	static constexpr unsigned long C_ID = 0x197E6FACE19DEC5;
	Wrapper<BuilderPort> c_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_BUILDERPORT_H_ */
