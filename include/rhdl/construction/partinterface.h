#ifndef PARTINTERFACE_H
#define PARTINTERFACE_H

#include <rhdl/construction/boundinterface.h>
#include <string>

//class std::string;

namespace rhdl {

class Interface;
class PartHandle;

class PartInterface : public BoundInterface
{
public:
	PartInterface(const PartHandle &part, const Interface *interface);

	PartInterface operator[] (const std::string &iname) const;

	void operator=(const BoundInterface &other) {BoundInterface::operator=(other);}
	void operator=(const PartInterface &other) {BoundInterface::operator=(other);}

	const Interfacible &owner() const override;
	const PartHandle &part_;
};

}

#endif // PARTINTERFACE_H
