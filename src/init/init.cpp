#include "init.h"
#include "lib/lib.h"

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

void try_init() {
	if (state == 3)
		lib_init();
}

}}
