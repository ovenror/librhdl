#ifndef ENTITYHANDLE_H
#define ENTITYHANDLE_H

#include <rhdl/construction/connectibleentity.h>
#include <rhdl/construction/entityinterface.h>
#include <rhdl/construction/interfacible.h>
#include "entity/entity.h"

#include "interface/iplaceholder.h"

#include "simulation/fast/fastsimfactory.h"

#include "representation/behavioral/timedbehavior.h"
#include "representation/structural/structural.h"

#include <string>
#include <memory>

namespace rhdl {

class Interface;
class PartHandle;
class Net;

class EntityHandle : public ConnectibleEntity<Interfacible> {
public:
	EntityHandle(const Entity &entity);
	virtual ~EntityHandle() {}

	EntityInterface operator[] (const std::string &iname) const;
	EntityInterface operator[] (const Interface *iface) const;

	bool connect(const Interface *iface, const PartHandle &to, const Interface *to_iface) const override;
	bool connect(const Interface *iface, const EntityHandle &to, const Interface *to_iface) const override;

	bool get_connected(const Interface *iface, const Interfacible &from, const Interface *from_iface) const override;

	Net &net() const;
	virtual const Entity &entity() const = 0;

	template<class siface_class, class internal_state_class>
	void addTimedBehavior(
			std::function<void(FastSim<siface_class, internal_state_class> &)> simfunc,
			std::function<void(FastSim<siface_class, internal_state_class> &)> procfunc = [](FastSim<siface_class, internal_state_class> &){},
			std::initializer_list<std::map<const char *, std::vector<bool> >::value_type> initial = {}
			)
	{
		entity().addRepresentation(std::move(TimedBehavior(entity(), timing_, simfunc, procfunc, initial)));
	}

	template<class siface_class, class internal_state_class>
	void addFunctionalBehavior(
			std::function<void(
				FastSim<siface_class, internal_state_class> &)>
			simfunc)
	{
		entity().addRepresentation(std::move(TimedBehavior(entity(), nullptr, simfunc)));
	}

	virtual void finalize() = 0;

protected:
	bool finalizeOnce();

private:
	const Timing* timing_;
	std::unique_ptr<Structural> structure_;
	bool finalized_;
};

}

#endif // ENTITYREF_H
