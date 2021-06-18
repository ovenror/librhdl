#include "einverter.h"

#include "interface/isingle.h"
#include "interface/icomposite.h"

#include "representation/behavioral/timedbehavior.h"

#include "simulation/invertersim.h"

#include "construction/buildsim.h"

#include <tuple>
#include <memory>

using namespace rhdl;

using behavioral::TimedBehavior;

EInverter::EInverter()
	: Entity("Inverter", {
			new ISingle ("in", Interface::Direction::IN, false),
			new ISingle ("out", Interface::Direction::OUT, false)
		})
{
	const Timing &timing = addTiming();

	makeNetlist(timing);
	makeTimedBehavior(timing);
}

void EInverter::makeNetlist(const Timing &timing)
{
	using netlist::Netlist;
	using netlist::VertexRef;

	auto pnetlist = std::make_unique<Netlist>(*this, nullptr, &timing);

	Netlist::Graph &graph = pnetlist -> graph_;
	Netlist::Interface &interface = pnetlist -> interface_;

	VertexRef vin = graph.addVertex();
	VertexRef vout = graph.addVertex();

	graph.connect(vin, vout);

	interface[(ISingle *) interface_["in"]] = vin;
	interface[(ISingle *) interface_["out"]] = vout;

	addRepresentation(std::move(pnetlist));
}

void EInverter::makeTimedBehavior(const Timing &timing)
{
	using iface = INTERFACE(SINGLE(in), SINGLE(out));

	TimedBehavior behavior(*this, &timing,
		FASTSIMFUNC(iface, NO_STATE)
		{
			SIMGET(out) = !SIMGET(in);
			SIMCLR(in);
		}
	);

	addRepresentation<TimedBehavior>(std::move(behavior));
}

void EInverter::makeFunctionalBehavior()
{
	using iface = INTERFACE(SINGLE(in), SINGLE(out));

	TimedBehavior behavior(*this, nullptr,
		FUNCTION(iface)
		{
			SIMGET(out) = !SIMGET(in);
			SIMCLR(in);
		}
	);

	addRepresentation<TimedBehavior>(std::move(behavior));
}

rhdl::EInverter::operator std::string() const
{
	return "(EInverter)";
}

