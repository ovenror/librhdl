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

	ISingle *clone() const override
	{
		return new ISingle (*this);
	}

	const Interface *get(const std::string &name) const override {std::ignore = name; return nullptr;}
	//const Interface *operator[] (const std::string &iname) const override {return nullptr;}
	const char *const *ls() const override {return nullptr;}

	bool eq_inner_names (const Interface &other) const override;

protected:
	CResult eq_struct_int (const Interface &other, const Predicate2 &predicate) const override;
	CResult eq_struct_int (const ISingle &other, const Predicate2 &predicate) const override;

private:
	friend class CSingle;
	friend class SetOpenVisitor;
};

}

#endif // ISINGLE_H
