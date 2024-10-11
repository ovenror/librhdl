#ifndef CONNECTIBLE_H
#define CONNECTIBLE_H

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/error.h>

#include <string>
#include <utility>

namespace rhdl {

class Interfacible;
class Interface;
class ConnectionPredicate;

namespace structural { namespace builder {
class Port;
class StructureBuilder;
}}

class Connectible
{
public:
	Connectible();
	virtual ~Connectible() {}

	const Connectible &operator>>(const Connectible &to) const;
	const Connectible &operator<<(const Connectible &from) const;
	Connectible &operator=(const Connectible &);

private:
	virtual structural::builder::Port &port() const = 0;
};

}

#endif // CONNECTIBLE_H
