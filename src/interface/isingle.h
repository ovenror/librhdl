#ifndef ISINGLE_H
#define ISINGLE_H

#include "interface.h"
#include "interface/cresult/csingle.h"
#include "../util/visitable.h"

namespace rhdl {

class ISingle : public Interface::VisitableChild<ISingle>
{
public:
	ISingle (const std::string &name, Direction dir, bool open = false);

	void setOpen() const;
	bool is_open() const {return c_.content().single.open;}
	Direction direction() const {return static_cast<Direction>(c_.content().single.dir);}
	CompositeDirection compositeDirection() const override {return CompositeDirection(direction());}
	SingleDirection preferredDirection() const override {return direction();}

	ISingle *clone(const std::string &newName) const override
	{
		return new ISingle (newName, direction(), is_open());
	}

	const Interface *get(const std::string &name) const override {std::ignore = name; return nullptr;}
	//const Interface *operator[] (const std::string &iname) const override {return nullptr;}

	bool eq_inner_names (const Interface &other) const override;

private:
	friend class CSingle;
};

}

#endif // ISINGLE_H
