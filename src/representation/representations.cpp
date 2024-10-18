#include "representations.h"
#include "representationtype.h"

#include "representation/behavioral/behavior.h"
#include "representation/behavioral/functionalbehavior.h"
#include "representation/behavioral/timedbehavior.h"
#include "representation/structural/structure.h"
#include "representation/spatial/tree/treemodel.h"
#include "representation/blocks/blocks.h"
#include "representation/txt/commands.h"

#include "init/init.h"

#include <cxxabi.h>

namespace rhdl {

decltype(Representations::validTypeIDs) Representations::validTypeIDs;

template <class T, class... Types>
struct Representations::MkObjects<List<T, Types...>> {
	std::vector<RepresentationType> objects_;

	static void insertObject(std::vector<RepresentationType> &objects) {
		std::string name = typeid(T).name();
		auto pos = name.find_last_of("0123456789");
		name = name.substr(pos + 1);
		name = name.substr(0, name.length() - 1);
		objects.push_back(std::move(RepresentationType(objects.size(), name)));
		MkObjects<List<Types...>>::insertObject(objects);
	}

	MkObjects() {
		insertObject(objects_);
	}

	std::vector<RepresentationType> objects() {
		return std::move(objects_);
	}
};

template <>
struct Representations::MkObjects<List<InvalidRepresentation>> {
	static void insertObject(std::vector<RepresentationType> &objects)
	{
		objects.push_back(RepresentationType(objects.size(), "Invalid"));
	}
};

Representations::Representations()
		: objects_(std::move(MkObjects<Types>().objects()))
{
	init::reps_ready();
}

Representations representations;

}
