#ifndef rhdl_TM_WIRES_H
#define rhdl_TM_WIRES_H

#include <vector>
#include <memory>
#include <functional>
#include "representation/blocks/blocks.h"
#include "connection.h"
#include "container.h"
#include "crosser.h"
#include "collectedwire.h"

namespace rhdl {
namespace TM {

class Container;
class SingleWire;

class Wires : public Container, public Crosser
{
public:
	using Collection = std::vector<std::unique_ptr<CollectedWire> >;

	Wires(Container &owner, bool vertical, bool anchors = false);

	Node *getNode() override {return owner_.getNode();}
	NodeGroup *getNodeGroup() override {return owner_.getNodeGroup();}

	//Layer *getLayer() override {return owner_.getLayer();}
	const Layer *getLayer() const {return owner_.getLayer();}

	const TreeModel &getModel() const {return owner_.getModel();}

	blocks::Blocks::index_t xpos() const;
	blocks::Blocks::index_t ypos() const;

	CollectedWire &make();
	CollectedWire &make(const std::shared_ptr<Connection> &connection);
	virtual std::unique_ptr<SingleWire> release(const Wire &wire);

	CollectedWire &findOrMake(const std::shared_ptr<Connection> &connection);
	CollectedWire &find(const Connection &connection) const;

	const Collection &collection() const {return collection_;}

	Collection::iterator begin() {return collection_.begin();}
	Collection::iterator end() {return collection_.end();}
	Collection::const_iterator cbegin() const {return collection_.cbegin();}
	Collection::const_iterator cend() const {return collection_.cend();}
	Collection::reverse_iterator rbegin() {return collection_.rbegin();}
	Collection::reverse_iterator rend() {return collection_.rend();}
	Collection::const_reverse_iterator crbegin() const {return collection_.crbegin();}
	Collection::const_reverse_iterator crend() const {return collection_.crend();}
	bool empty() {return collection_.empty();}

	bool isConnectedTo(const Wire &w) const override;
	const Wire *getWireConnectedTo(const Wire &w) const override;
	Wire *getWireConnectedTo(const Wire &w) override;

	CollectedWire *findMax1(const TM::Connection &c) const;
	std::vector<CollectedWire *> findConnected(const TM::Connection &c) const;

	blocks::Blocks::index_t maxRelPos() const;
	bool noNonAnchors() const;
	std::vector<CollectedWire *> top() const;
	std::vector<CollectedWire *> bottom() const;
	std::vector<CollectedWire *> wiresAt(blocks::Blocks::index_t position) const;

	void tryInsertCrosser(Wire &crosser) override;
	void replaceCrosser(Wire &newCrosser) override;
	bool mayBeCrossed(const Wire &wire, blocks::Blocks::index_t at) const override;

	void applyToWires(std::function<void(Wire &)> f) const;

	virtual void computePositions();

	virtual bool dropCollision(const CollectedWire &dropped, blocks::Blocks::index_t at, const CollectedWire &other) const;

	unsigned int numIsolatedUpperInputCrossings(const CollectedWire &wire) const;

	virtual const std::vector<const Wire *> debug_getWires() const override;

	Container &owner_;
	bool anchors_;
	Collection collection_;

protected:
	unsigned int newIndex();
	virtual CollectedWire &add(std::unique_ptr<CollectedWire> &&newWire);

	unsigned int nextIndex_;
};


}
}
#endif // rhdl_TM_WIRES_H
