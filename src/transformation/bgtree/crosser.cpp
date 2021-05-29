#include "crosser.h"
#include "collectedwire.h"

namespace rhdl {
namespace TM {

Crosser::Crosser(bool vertical) :
	vertical_(vertical)
{
}

Crosser::Crosser(CollectedWire &&dying) : Crosser(dying.vertical())
{
	crossers_ = dying.getCrossers(); //yes, copy!

	for (Crosser *crosser : getCrossers()) {
		//assert(crosser -> crossers_.insert(this).second);
		crosser -> addCrosser_oneway(*this);
	}
}

void Crosser::addCrosser(Crosser &crosser)
{
	addCrosser_oneway(crosser);
	crosser.addCrosser_oneway(*this);
}

bool Crosser::isCrosser(const Crosser &crosser) const
{
	bool result = isCrosser_internal(crosser);
	assert (result == crosser.isCrosser_internal(*this));
	return result;
}

bool Crosser::isCrosser_internal(const Crosser &crosser) const
{
	return (crossers_.find(const_cast<Crosser *>(&crosser)) != crossers_.end());
}

void Crosser::addCrosser_oneway(Crosser &crosser)
{
	assert (crosser.vertical_ != vertical_);
	auto result = crossers_.insert(&crosser);
	assert (result.second);
}

}
}
