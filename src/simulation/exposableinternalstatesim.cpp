#include "exposableinternalstatesim.h"

namespace rhdl {

ExposableInternalStateSim::ExposableInternalStateSim()
{

}

void ExposableInternalStateSim::processInput()
{
	for (const auto &kv : input()) {
		if (kv.second)
			setInternal(kv.first);
	}
}

}
