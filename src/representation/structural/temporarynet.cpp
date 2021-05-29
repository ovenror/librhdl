#include "temporarynet.h"
#include "construction/parthandle.h"
#include "entity/entity.h"
#include <iostream>

namespace rhdl {

TemporaryNet::TemporaryNet()
	: stateless_(true)
{}

TemporaryNet::~TemporaryNet()
{
}

PartIdx TemporaryNet::add(const Entity *what, PartHandle *partobj)
{
	PartIdx part = net_.add(what);
	stateless_ = stateless_ && what -> isStateless();
	parthandles_.push_back(partobj);
	return part;
}

void TemporaryNet::absorb_dispatch(Net &victim)
{
	victim.meld_into(*this);
}

void TemporaryNet::insert_connection(const Net::Connection &connection)
{
	Net::insert_connection_into(connection, net_);
}


void TemporaryNet::meld_into(TemporaryNet &common)
{
	update_parthandles(common);

	for (PartHandle *part : parthandles_) {
		common.parthandles_.push_back(part);
	}

	complete_meld_into(common.net_);
}

void TemporaryNet::meld_into(Net &common)
{    
	update_parthandles(common);
	complete_meld_into(common);
}

void TemporaryNet::update_parthandles(Net &common)
{
	PartIdx offset = common.parts().size();

	for (PartHandle *part : parthandles_)
		part -> update_after_meld(offset, &common);
}

void TemporaryNet::complete_meld_into(Net &common)
{
	PartIdx offset = common.parts().size();

	for (const Entity *e : net_.parts())
		common.add(e);

	for (Connection c : net_.connections()) {
		Port &out = c.first;
		Port &in = c.second;

		PartIdx p0 = out.first + offset;
		const Interface *i0 = out.second;

		PartIdx p1 = in.first + offset;
		const Interface *i1 = in.second;

		Net::insert_connection_into({Port(p0,i0),Port(p1,i1)}, common);
	}

	delete this;
}

}
