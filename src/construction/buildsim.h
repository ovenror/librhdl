#ifndef FASTSIMsdhfjkl_H
#define FASTSIMsdhfjkl_H

#include "util/static_strings.h"
#include "simulation/fast/sicomposite.h"
#include "simulation/fast/sisingle.h"
#include "simulation/fast/fastsimfactory.h"
#include "simulation/fast/traversal.h"

#define SINGLE(name) ::rhdl::behavioral::SISingle<TOSTRING(name)>
#define COMPOSITE(name, ...) ::rhdl::behavioral::SIComposite<TOSTRING(name), __VA_ARGS__>

#define INTERFACE(...) COMPOSITE(, __VA_ARGS__)

#define SIMSTATE(sim, qname) sim.iface<Split<TOSTRING(qname)>::RESULT>()
#define SIMGET(qname) SIMSTATE(sim_instance, qname)
#define SIMSET(qname) SIMGET(qname) = true
#define SIMCLR(qname) SIMGET(qname) = false
#define SIMINT sim_instance.internal_state_

#define FASTSIMFUNC(siface_class, internal_state_class) \
(::rhdl::behavioral::FastSimFactory<siface_class, internal_state_class>::StepFunc) \
[](::rhdl::behavioral::FastSim<siface_class, internal_state_class> &sim_instance) -> void

#define FUNCTION(siface_class) FASTSIMFUNC(siface_class, NO_STATE)


#define SIMNOP std::ignore = sim_instance

#endif // FASTSIM_H
