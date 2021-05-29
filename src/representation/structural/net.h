#ifndef NET_H
#define NET_H

#include <utility>
#include <vector>
#include <string>

namespace rhdl {

class Entity;
class Interface;
class ISingle;
class TemporaryNet;
typedef std::size_t PartIdx;
enum {containingPartIdx = 0};

class Net
{
public:
	typedef std::pair <PartIdx, const Interface *> Port;
	typedef std::pair <Port, Port> Connection;

	Net();
	virtual ~Net();

	virtual PartIdx add (const Entity *what) = 0;

	virtual const std::vector <const Entity *> &parts() const = 0;
	virtual const std::vector <Connection> &connections() const = 0;

	virtual bool connect (PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface) = 0;
	//virtual bool connect (Port from, Port to);

	void absorb(Net &victim);
	virtual void meld_into(TemporaryNet &common);

	std::string portToString(const Port &port) const;
	std::string connectionToString(const Connection &connection) const;
	operator std::string() const;

protected:
	virtual void meld_into(Net &common);
	static void insert_connection_into (const Connection &connection, Net &into);
	virtual void insert_connection (const Connection &connection) = 0;
	virtual void absorb_dispatch(Net &victim);

	static void check(const Net::Connection &greater, const Net::Connection &lesser);
	static void check(const Net::Port &super, const Net::Port &sub, const Net::Port &different1, const Net::Port &different2);
};


bool operator>=(const Net::Port &super, const Net::Port &sub);
bool operator>=(const Net::Connection &super, const Net::Connection &sub);

}


#endif // NET_H
