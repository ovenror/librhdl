#ifndef FIXOVERLONGWIRES_H
#define FIXOVERLONGWIRES_H

#include <tuple>
#include <vector>
#include <map>
#include <forward_list>

namespace rhdl {

class ISingle;

namespace spatial {

class Wire;
class Connector;
class Connection;
class Node;

using Link = std::pair<const Connector *, const Connector *>;
using Links = std::vector<Link>;
using WorkingAndBrokenLinks = std::pair<Links, Links>;
using ConnectionLinks = std::map<const Connection *, WorkingAndBrokenLinks>;
enum class FixConnectionResult {UNCHANGED, FIXED, BROKEN};

void createSuperSegments(const Connection &connection);

FixConnectionResult fixConnection(const Links &links);

const Connector &getOutputConnector(const Node &node);
const Connector &getInputConnector(const Node &node);
const Connector &getIFaceConnector(const Wire &ifaceWire);

}} //rhdl::spatial


#endif // FIXOVERLONGWIRES_H
