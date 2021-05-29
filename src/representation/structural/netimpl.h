#ifndef NET_IMPL_H
#define NET_IMPL_H

#include <vector>
#include <string>
#include "net.h"

namespace rhdl {

class Entity;
class TemporaryNet;

class NetImpl : public Net
{
public:
	NetImpl();
	~NetImpl();

	PartIdx add (const Entity *what) override;

	const std::vector <const Entity *> &parts() const override {return parts_;}
	const std::vector <Connection> &connections() const override {return connections_;}

	bool connect (PartIdx from, const std::string &from_interface, PartIdx to, const std::string &to_interface);
	bool connect (PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface);

	void makeFlatConnections();

protected:
	void insert_connection (const Connection &connection) override;

	std::vector <Connection> connections_;
	std::vector <const Entity *> parts_;   
};

}

#endif // NET_IMPL_H
