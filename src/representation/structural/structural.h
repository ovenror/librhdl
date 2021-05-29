#ifndef ECOMPOSITE_H
#define ECOMPOSITE_H

#include "netimpl.h"
#include "simulation/simfactory.h"
#include "representation/representationbase.h"

namespace rhdl {

class IPlaceholder;

class Structural : public RepresentationBase<Structural>, public NetImpl
{
public:
	typedef NetImpl::Port Port;
	typedef NetImpl::Connection Connection;
	typedef std::pair <PartIdx, const ISingle *> FlatPort;
	typedef std::pair <FlatPort, FlatPort> FlatConnection;

public:
	Structural(const Entity &entity, const Representation *parent, const Timing *timing);

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

	PartIdx add (const Entity *what) override;

	bool expose (PartIdx part, const Interface *part_interface, const Interface *ext_interface);

#if 0
	bool expose (PartIdx part, const Interface *part_interface, const std::string &ext_interface_name = "");
	bool expose (PartIdx part, const std::string &part_interface_name, const std::string &ext_interface_name = "");
#endif

	bool connect (PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface) override;
	bool connect (PartIdx from, const std::string &from_interface, PartIdx to, const std::string &to_interface)
	{
		return NetImpl::connect(from, from_interface, to, to_interface);
	}

	operator std::string() const;

	static bool isContainer(const PartIdx pidx)
	{
		return pidx == containingPartIdx;
	}

	static bool isExternal(const Port &p)
	{
		return isContainer(p.first);
	}

	static bool isExposure(const Connection &c)
	{
		assert (!isExternal(c.first));
		return isExternal(c.second);
	}

	std::string exposureToString(const Connection &exp) const;

	const std::vector<FlatConnection> &flatConnections() const;
	void makeFlatConnections() const;
	PartIdx This() {return parts().size() - 1;}

private:
	friend class EntityHandle;
	friend class CComposite;

	mutable bool has_flat_connections_;
	mutable std::vector <FlatConnection> flat_connections_;
};



}

#endif // ECOMPOSITE_H

