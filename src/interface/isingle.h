#ifndef ISINGLE_H
#define ISINGLE_H

#include "interface.h"
#include "cresult/csingle.h"

#include "c_api/referencedcvalue.h"
#include "c_api/cvalueobject.h"

#include "util/visitable.h"
#include "util/lexicaldictionary.h"

namespace rhdl {

class ISingle : public Interface::VisitableChild<ISingle>, CValueContainer
{
public:
	ISingle (const std::string &name, Direction dir);
	virtual ~ISingle();

	Direction direction() const {return static_cast<Direction>(c_ptr() -> single.dir);}
	CompositeDirection compositeDirection() const override {return CompositeDirection(direction());}
	SingleDirection preferredDirection() const override {return direction();}

	ISingle *clone(const std::string &newName) const override
	{
		return new ISingle (newName, direction());
	}

	const Interface *get(const std::string &name) const override {std::ignore = name; return nullptr;}
	//const Interface *operator[] (const std::string &iname) const override {return nullptr;}

	bool eq_inner_names (const Interface &other) const override;

private:
	friend class CSingle;

	const CObject &add(const CValue &) override;
	const CObject &add_after_move(const CValue &) override;

	LexicalDictionary<const CObject *> dict_;
	ReferencedCValue<rhdl_direction> dir_;
};

}

#endif // ISINGLE_H
