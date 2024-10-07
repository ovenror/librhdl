#include "init.h"
#include "lib/lib.h"
#include "c_api/rootnamespace.h"

namespace rhdl {
namespace init {

int state;

void transformer_ready()
{
	state |= 1;
	try_init();
}

void lib_ready()
{
	state |= 2;
	try_init();
}

void root_ready()
{
	state |= 4;
	try_init();
}

void try_init() {
	if (state == 7) {
		RootNamespace::init();
		lib_init();
	}
}

}

}
