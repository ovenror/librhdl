#include "representation/structural/structure.h"
#include "representation/spatial/tree/treemodel.h"
#include "transformations.h"

#include "bgtree.h"
#include "transformation/defaulttransformation.h"

/*
 * Include available transformations and register them in the constructor
 * at the bottom. For default transformations, the concerned types (source
 * and destination) have to be known (included).
 */

// transformations
#include "transformation/structure2netlist.h"
// destination types
#include "representation/txt/commands.h"

/*
 * ---
 */


namespace rhdl {

template <class... types>
struct Transformations::Init {
	static void init(Transformations &) {}
};

template <class type, class... types>
struct Transformations::Init<type, types...> {
	static void init(Transformations &that)
	{
		that.objects_.push_back(std::make_unique<const type>());
		Init<types...>::init(that);
	}
};

/*
 * Register transformations here.
 */
Transformations::Transformations()
{
	Init<
		StructureToNetlist,
		DefaultTransformation<netlist::Netlist, spatial::TreeModel>,
		BGTree,
		DefaultTransformation<blocks::Blocks, txt::Commands>
	>::init(*this);
}

}
