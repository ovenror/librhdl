#ifndef CONTAINER_H
#define CONTAINER_H

#include "util/cached.h"
#include <functional>

#include <boost/mpl/insert.hpp>
#include "../tree/element.h"
#include "../tree/wire.h"

namespace rhdl {
namespace spatial {

class TreeModel;
class Layer;
class NodeGroup;
class Node;

class Container : public Element
{
public:
	using Collection = std::vector<std::unique_ptr<Container> >;

	Container(unsigned int index);

	virtual const Node* getNode() const {return nullptr;}
	virtual Node* getNode() {return nullptr;}
	virtual const NodeGroup *getNodeGroup() const;
	virtual NodeGroup *getNodeGroup();
	virtual const Layer *getLayer() const;
	virtual Layer *getLayer();
	virtual const TreeModel &getModel() const;
	//virtual TreeModel &getModel();

	blocks::index_t width() const {return width_;}
	blocks::index_t height() const {return height_;}

	virtual blocks::index_t xpos() const = 0;
	virtual blocks::index_t ypos() const = 0;

protected:
	blocks::index_t width_;
	blocks::index_t height_;

	template <class ELEMENT>
	static void insert(std::unique_ptr<ELEMENT> element, std::vector<std::unique_ptr<ELEMENT> > &collection);

	template <class ELEMENT>
	static void erase(ELEMENT &element, std::vector<std::unique_ptr<ELEMENT> > &collection);

private:
	friend std::ostream &operator<<(std::ostream &os, const Container &c);
};

std::ostream &operator<<(std::ostream &os, const Container &c);

}
}

#endif // CONTAINER_H
