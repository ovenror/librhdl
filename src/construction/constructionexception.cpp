#include <rhdl/construction/constructionexception.h>
#include <cassert>
#include <sstream>

namespace rhdl {

ConstructionException::ConstructionException(Errorcode code, const std::string &info) :
	code_(code)
{
	if (info == "")
		return;

	std::stringstream strm;
	strm << whatcode() << std::endl << info;
	info_ = strm.str();
}

ConstructionException::~ConstructionException() {
}

const char *ConstructionException::whatcode() const noexcept
{
	switch (code_) {
	case Errorcode::E_NO_ERROR: return "(no error)";

	case Errorcode::E_CANNOT_GET_ANONYMOUS_INTERFACE: return "Cannot look a possibly existing anonymous interface up by name";
	case Errorcode::E_NO_SUCH_INTERFACE: return "Interface does not exist.";

	case Errorcode::E_COMPATIBLE_INTERFACES_NOT_FOUND: return "No compatible interfaces found.";
	case Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES: return "Multiple combinations of compatible interfaces found. Connection is ambiguous.";
	case Errorcode::E_ILLEGAL_RECONNECTION: return "Existing connections or subsets thereof may not be connected again.";
	case Errorcode::E_ILLEGAL_PASSTHROUGH: return "Direct passthroughs are (currently) forbidden.";

	case Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY: return "Cannot add stateful component to structural representation of functional entity.";

	case Errorcode::E_NETLIST_CONTAINS_CYCLES: return "The netlist (that was generated from the representation) you tried to add contains cycles, although the entity should be stateless";
	case Errorcode::E_EMPTY_INTERFACE: return "The entities top interface or another composite interface is empty.";

	case Errorcode::E_NO_SUCH_ENTITY: return "No entity with that name exists.";
	case Errorcode::E_ENTITY_EXISTS: return "An entity with that name already exists.";
	case Errorcode::E_NO_SUCH_MEMBER: return "No member with that name exists.";
	case Errorcode::E_MEMBER_EXISTS: return "An member with that name already exists.";

	case Errorcode::E_WRONG_NAME: return "Fast Simulator interface does not match entity interface: wrong name.";
	case Errorcode::E_WRONG_TYPE: return "Fast Simulator interface does not match entity interface: wrong type.";

	default:
		assert(0);
	}

	return "";
}

const char *ConstructionException::what() const noexcept
{
	if (info_ == "")
		return whatcode();
	else
		return info_.c_str();
}

}
