#ifndef REPRESENTATIONS_H
#define REPRESENTATIONS_H

#include "representationtypeid.h"
#include "util/list.h"

#include <array>
#include <bitset>

namespace rhdl {

namespace behavioral {
class TimedBehavior;
class FunctionalBehavior;
}

namespace structural {
class CompressedStructure;
class Structure;
}

namespace netlist {
class Netlist;
}

namespace spatial {
class TreeModel;
}

namespace blocks {
class Blocks;
}

namespace txt {
class Commands;
}

class InvalidRepresentation;

class Representations
{
public:
	using Types = List<
			behavioral::TimedBehavior, behavioral::FunctionalBehavior,
			structural::CompressedStructure, structural::Structure,
			netlist::Netlist,
			spatial::TreeModel,
			blocks::Blocks,
			txt::Commands,
			InvalidRepresentation>;
	using RepTypeID = RepresentationTypeID;

	Representations();

private:
	template <RepTypeID... typeIDs>
	struct TypeIDList;

	template <class typelist, RepTypeID startID = 0, RepTypeID... typeIDs>
	struct MkValidIDLIst;

	template <class Want, RepTypeID id, class... types>
	struct TypeToIDHelper;

	template <class Want, RepTypeID id, class Have, class... types>
	struct TypeToIDHelper<Want, id, Have, types...>: TypeToIDHelper<Want, id+1, types...> {};

	template <class Found, RepTypeID id, class... types>
	struct TypeToIDHelper<Found, id, Found, types...> : std::integral_constant<RepTypeID, id> {};

	template <class Want, RepTypeID id, class... types>
	struct TypeToIDHelper<Want, id, List<types...>>: TypeToIDHelper<Want, id, types...> {};

	template <RepTypeID startID, RepTypeID... typeIDs>
	struct MkValidIDLIst<List<InvalidRepresentation>, startID, typeIDs...> {
		using list = TypeIDList<typeIDs...>;
		static constexpr RepTypeID count = 0;
	};

	template <class type, class... types, RepTypeID startID, RepTypeID... typeIDs>
	struct MkValidIDLIst<List<type, types...>, startID, typeIDs...> {
		using NextIteration = MkValidIDLIst<List<types...>, startID + 1, typeIDs..., startID>;
		using list = typename NextIteration::list;
		static constexpr unsigned int count = NextIteration::count + 1;
	};

	template <unsigned int validCount, class ValidTypeIDs>
	struct MkArray;

	template <unsigned int validCount, RepTypeID... validTypeIDs>
	struct MkArray<validCount, TypeIDList<validTypeIDs...>> {
		using Type = std::array<RepTypeID, validCount>;
		static constexpr Type array = {validTypeIDs...};
	};

	using IDGenerator = MkValidIDLIst<Types>;
	using ArrayGenerator = MkArray<IDGenerator::count, typename IDGenerator::list>;

public:
	template <class type>
	using TypeToID = TypeToIDHelper<type, 0, Types>;
	static constexpr auto validTypeIDs = ArrayGenerator::array;
	using TypeSet = std::bitset<validTypeIDs.size()>;
	static constexpr RepTypeID INVALID = TypeToID<InvalidRepresentation>::value;
};

}

#endif // REPRESENTATIONS_H
