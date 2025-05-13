/*
 * indexcreator.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#include "portscreator.h"
#include "complexport.h"
#include "simpleport.h"
#include "interface/icomposite.h"
#include "interface/isingle.h"

#include <cassert>
#include <memory>

namespace rhdl::structural::builder {

PortsCreator::PortsCreator(Element &element)
	: element_(element)
{}

PortsCreator::~PortsCreator() {}

void PortsCreator::visit(const ISingle& i) {
	assert (enclosed_);
	auto result = builder_c_ ?
			std::make_unique<SimplePort>(element_, i, std::move(builder_c_)) :
			std::make_unique<SimplePort>(element_, i);
	enclosed_ -> add(std::move(result));
}

void PortsCreator::visit(const IComposite& i) {
	assert (enclosed_);

	auto enclosed_saved = enclosed_;
	Port::WPtr builder_c = std::move(builder_c_);
	ComplexPort::Enclosed enclosed;
	createEnclosed(i, enclosed);
	auto result = builder_c ?
			std::make_unique<ComplexPort>(element_, i, std::move(enclosed), std::move(builder_c)) :
			std::make_unique<ComplexPort>(element_, i, std::move(enclosed));
	enclosed_saved -> add(std::move(result));
	enclosed_ = enclosed_saved;
}

ComplexPort PortsCreator::create(const IComposite &i, const std::string *name)
{
	ComplexPort::Enclosed enclosed;
	createEnclosed(i, enclosed);
	return ComplexPort(element_, i, std::move(enclosed), name);
}

std::unique_ptr<ExistingPort> PortsCreator::create(
		const Interface &i, std::unique_ptr<Wrapper<Port>> &&builder_c)
{
	builder_c_ = std::move(builder_c);

	ComplexPort::Enclosed dummy;
	enclosed_ = &dummy;

	i.accept(*this);

	return std::move(dummy.erase(dummy.begin()));
}

void PortsCreator::createEnclosed(const IComposite &i,
		ComplexPort::Enclosed &set)
{
	enclosed_ = &set;

	for (const Interface *c : i)
		c -> accept(*this);
}

} /* namespace rhdl::structural::builder */
