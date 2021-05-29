#ifndef ENTITYINTERFACE_H
#define ENTITYINTERFACE_H

#include <rhdl/construction/boundinterface.h>
#include <rhdl/construction/connectibleentity.h>

namespace rhdl {

class Entity;
class EntityHandle;
class Interface;
class PartInterface;

class EntityInterface : public ConnectibleEntity<BoundInterface>
{
public:
	using Super = ConnectibleEntity<BoundInterface>;

	EntityInterface(const EntityHandle &ehandle, const Interface *interface);

	void operator=(const PartInterface &other) const;
	EntityInterface operator[] (const std::string &iname) const;

	const Interfacible &owner() const override;

private:
	void operator=(EntityInterface &to);

	const EntityHandle &ehandle_;
};

}

#endif // ENTITYINTERFACE_H
