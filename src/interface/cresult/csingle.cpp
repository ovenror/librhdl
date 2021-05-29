#include "csingle.h"
#include "interface/isingle.h"

namespace rhdl {


CSingle::CSingle(const ISingle &from, const ISingle &to, const Predicate &predicate) :
	BASE(from, to, predicate)
{
}

CSingle::~CSingle()
{
}

void CSingle::eval_int() const
{
	bool dir = (from_.direction() == to_.direction()) != predicate_.samedir_;
	bool open = from_.is_open() && to_.is_open() && !predicate_.samedir_;
	bool op = !dir && predicate_.check_first_dir_ && from_.direction() != predicate_.first_dir_;

	if (!(dir || open || op)) {
		ec_ = Errorcode::E_NO_ERROR;
		success_ = true;
		return;
	}

	success_ = false;

	if ((dir || op) + open > 1)
		ec_ = Errorcode::E_INTERFACES_NOT_COMPATIBLE;
	else if (dir)
		ec_ = Errorcode::E_INCOMPATIBLE_DIRECTIONS;
	else if (open)
		ec_ = Errorcode::E_OPEN_TO_OPEN;
	else if (op)
		ec_ = Errorcode::E_DIRECTION_OPPOSES_OPERATOR;

	if (dir) {
		if (predicate_.samedir_)
			msg_ << "* opposite directions, should be the same" << std::endl;
		else
			msg_ << "* same directions, should be opposite" << std::endl;
	}

	if (open)
		msg_<< "* both are open, which is not allowed with opposite directions" << std::endl;

	if (op)
		msg_ << "* direction of first should be " << predicate_.first_dir_;
		msg_ << " but is " << from_.direction() << std::endl;
}

}
