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
	enclosed_ -> add(std::make_unique<SimplePort>(element_, i));
}

void PortsCreator::visit(const IComposite& i) {
	assert (enclosed_);

	auto enclosed_saved = enclosed_;
	ComplexPort::Enclosed enclosed;
	createEnclosed(i, enclosed);
	enclosed_saved -> add(std::make_unique<ComplexPort>(element_, i, std::move(enclosed)));
	enclosed_ = enclosed_saved;
}

ComplexPort PortsCreator::create(const IComposite &i)
{
	ComplexPort::Enclosed enclosed;
	createEnclosed(i, enclosed);
	return ComplexPort(element_, i, std::move(enclosed));
}

std::unique_ptr<ExistingPort> PortsCreator::create(const Interface &i)
{
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
