#ifndef TEMPORARYNET_H
#define TEMPORARYNET_H

#include "net.h"
#include "netimpl.h"

namespace rhdl {

class PartHandle;

class TemporaryNet : public Net
{
public:
	TemporaryNet();
	~TemporaryNet();

	PartIdx add (const Entity *what) override {return net_.add(what);}
	PartIdx add (const Entity *what, PartHandle *phandle);

	void meld_into(Net &common) override;
	void meld_into(TemporaryNet &common) override;

	virtual const std::vector <const Entity *> &parts() const override {return net_.parts();}
	virtual const std::vector <Connection> &connections() const override {return net_.connections();}

	virtual bool connect (PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface) override
		{return net_.connect(from, from_interface, to, to_interface);}

protected:
	void absorb_dispatch(Net &victim) override;
	void insert_connection (const Connection &connection) override;

	void update_parthandles(Net &common);
	void complete_meld_into(Net &common);

private:
	std::vector<PartHandle *> parthandles_;
	NetImpl net_;
	bool stateless_;
};

}

#endif // TEMPORARYNET_H
