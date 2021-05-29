#include "transformations.h"

/*
 * Include available transformations and register them in the constructor
 * at the bottom.
 */

#include "bgtree/bgtree.h"
#include "transformation/structure2netlist.h"
#include "transformation/blocks2commands.h"
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
	Init<BGTree, StructureToNetlist, BlocksToCommands>::init(*this);
}

}
