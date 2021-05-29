#include "container.h"
#include "wire.h"
#include "node.h"
#include "nodegroup.h"
#include "layer.h"

#include <numeric>
#include <functional>
#include <cassert>
#include <memory>

namespace rhdl {
namespace TM {

Container::Container(unsigned int index) :
	Element(index)
{}

const NodeGroup *Container::getNodeGroup() const
{
	const Node *node = getNode();

	if (!node)
		return nullptr;

	return node -> getNodeGroup();
}

NodeGroup *Container::getNodeGroup()
{
	const Container *constThis = static_cast<const Container *>(this);
	const NodeGroup *constResult = constThis -> getNodeGroup();
	return const_cast<NodeGroup *>(constResult);
}

const Layer *Container::getLayer() const
{
	const NodeGroup *ng = getNodeGroup();

	if (!ng)
		return nullptr;

	return ng -> getLayer();
}

Layer *Container::getLayer()
{
	const Container *constThis = static_cast<const Container *>(this);
	const Layer *constResult = constThis -> getLayer();
	return const_cast<Layer *>(constResult);
}

const TreeModel &Container::getModel() const
{
	const Layer *l = getLayer();
	assert (l);
	return  l -> asContainer().getModel();
}

#if 0
TreeModel &Container::getModel()
{
	const Container *constThis = static_cast<const Container *>(this);
	const TreeModel &constResult = constThis -> getModel();
	return const_cast<TreeModel &>(constResult);
}
#endif

template <class ELEMENT>
void Container::insert(std::unique_ptr<ELEMENT> element, std::vector<std::unique_ptr<ELEMENT> > &collection)
{
	unsigned int index = element -> index();
	collection.insert(collection.begin() + index, std::move(element));

	for (
			auto ipelement = collection.begin() + index + 1;
			ipelement < collection.end();
			++ipelement
		 )
	{
		(*ipelement) -> incrementIndex();
	}
}

template <class ELEMENT>
void Container::erase(ELEMENT &element, std::vector<std::unique_ptr<ELEMENT> > &collection)
{
	unsigned int index = element.index_;
	assert (collection[index].get() == &element);

	collection.erase(collection.begin() + index);

	for (
			auto ipelement = collection.begin() + index;
			ipelement < collection.end();
			++ipelement
		 )
	{
		--((*ipelement) -> index_);
	}
}


template void Container::insert(std::unique_ptr<Node> element, std::vector<std::unique_ptr<Node> > &collection);
template void Container::insert(std::unique_ptr<NodeGroup> element, std::vector<std::unique_ptr<NodeGroup> > &collection);

std::ostream &operator<<(std::ostream &os, const Container &cc)
{
	Container &c = const_cast<Container &>(cc);

	if (c.getLayer()) os << "L" << c.getLayer() -> asContainer().index();
	if (c.getNodeGroup()) os << "G" << c.getNodeGroup() -> index();
	if (c.getNode()) os << "N" << c.getNode() -> index();

	return os;
}

}
}
