#ifndef FIXOVERLONGWIRES_H
#define FIXOVERLONGWIRES_H

#include <tuple>
#include <vector>
#include <map>
#include <forward_list>

namespace rhdl {

class ISingle;

namespace blocks { class Blocks; }

namespace TM {

class Wire;
class Connector;
class Connection;
class Node;

using Link = std::pair<const TM::Connector *, const TM::Connector *>;
using Links = std::vector<Link>;
using WorkingAndBrokenLinks = std::pair<Links, Links>;
using ConnectionLinks = std::map<const TM::Connection *, WorkingAndBrokenLinks>;

std::forward_list<const Connection *> fixBrokenConnections(
		const ConnectionLinks &connectionsLinks,
		blocks::Blocks &b);

const Connector &getOutputConnector(const Node &node);
const Connector &getInputConnector(const Node &node);
const Connector &getIFaceConnector(const Wire &ifaceWire);

}}


#endif // FIXOVERLONGWIRES_H
