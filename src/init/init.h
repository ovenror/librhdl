#ifndef INIT_H
#define INIT_H

#include "transformation/transformer.h"
#include "construction/library.h"

namespace rhdl {

class Library;
class Transformer;

namespace init
{
	extern int state;

	void transformer_ready();
	void lib_ready();
	void try_init();
};

}


#endif // INIT_H
