#include "misconstruction.h"
#include "rhdl/rhdl.h"
#include "macros.h"

static int inv2(char *sname, rhdl_structure_t **structure,
		rhdl_connector_t **i0in, rhdl_connector_t **i0out,
		rhdl_connector_t **i1in, rhdl_connector_t **i1out)
{
	rhdl_structure_t *s = rhdl_begin_structure(0, sname, F_CREATE_STATELESS);
	REQUIRE(s);

	if (structure)
		*structure = s;

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	rhdl_connector_t *i0 = rhdl_component(s, inverter);
	rhdl_connector_t *i1 = rhdl_component(s, inverter);
	REQUIRE(i0 && i1);

	*i0in = rhdl_select(i0, "in");
	*i0out = rhdl_select(i0, "out");
	*i1in = rhdl_select(i1, "in");
	*i1out = rhdl_select(i1, "out");

	return ACCEPT(i0in && i0out && i1in && i1out);
}

int incompatibleDirections()
{
	rhdl_structure_t *s;
	rhdl_connector_t *i0in, *i0out, *i1in, *i1out;

	REQUIRE(inv2("test1", &s, &i0in, &i0out, &i1in, &i1out) == SUCCESS);

	REQUIRE(rhdl_connect(i0out, i1out) == E_INCOMPATIBLE_DIRECTIONS);
	REQUIRE(rhdl_connect(i1out, i0out) == E_INCOMPATIBLE_DIRECTIONS);
	REQUIRE(rhdl_connect(i0in, i1in) == E_INCOMPATIBLE_DIRECTIONS);
	REQUIRE(rhdl_connect(i1in, i0in) == E_INCOMPATIBLE_DIRECTIONS);
	REQUIRE(rhdl_errno(s) == E_INCOMPATIBLE_DIRECTIONS);

	return SUCCESS;
}

int wrongOp()
{
	rhdl_connector_t *i0in, *i0out, *i1in, *i1out;

	REQUIRE(inv2("test2", 0, &i0in, &i0out, &i1in, &i1out) == SUCCESS);

	return ACCEPT(rhdl_connect(i0in, i0out) == E_DIRECTION_OPPOSES_OPERATOR);
}

int open2open()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test3", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *rsl = rhdl_entity(0, "RS_Latch");
	rhdl_connector_t *r0 = rhdl_component(s, rsl);
	rhdl_connector_t *r1 = rhdl_component(s, rsl);
	REQUIRE(r0 && r1);

	rhdl_connector_t *r0Q = rhdl_select(r0, "Q");
	rhdl_connector_t *r1S = rhdl_select(r0, "S");
	REQUIRE(r0Q && r1S);

	return ACCEPT(rhdl_connect(r0Q, r1S) == E_OPEN_TO_OPEN);
}

int ambiguousConnection()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test4", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *nand = rhdl_entity(0, "NAND");
	rhdl_connector_t *nand0 = rhdl_component(s, nand);
	REQUIRE(nand0);

	return ACCEPT(rhdl_connect(nand0, nand0) == E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);
}

int alreadyConnectedToOpen()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test5", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *nor = rhdl_entity(0, "NOR");
	rhdl_connector_t *nor0 = rhdl_component(s, nor);
	REQUIRE(nor0);

	rhdl_connector_t *nor_in0 = rhdl_select(nor0, "in0");
	rhdl_connector_t *nor_in1 = rhdl_select(nor0, "in1");
	REQUIRE(nor_in0 && nor_in1);

	REQUIRE_NOERR(rhdl_connect(nor0, nor_in0));

	REQUIRE(rhdl_connect(nor0, s -> connector) == E_ALREADY_CONNECTED_TO_OPEN);
	REQUIRE(rhdl_errno() == E_ALREADY_CONNECTED_TO_OPEN);
	REQUIRE(rhdl_connect(nor0, nor_in1) == E_ALREADY_CONNECTED_TO_OPEN);
	REQUIRE(rhdl_errno() == E_ALREADY_CONNECTED_TO_OPEN);

	return SUCCESS;
}

int statefulComponentInStatelessStructure()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test6", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *rsl = rhdl_entity(0, "RS_Latch");
	rhdl_connector_t *r = rhdl_component(s, rsl);
	REQUIRE(r);

	rhdl_connector_t *rS = rhdl_select(r, "S");
	REQUIRE(rS);

	return ACCEPT(rhdl_connect(s -> connector, rS) == E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY);

	/* got bored, forwent remainder */	
}

int introduceCycle()
{
	rhdl_structure_t *s;
	rhdl_connector_t *i0in, *i0out, *i1in, *i1out;

	REQUIRE (inv2("test8", &s, &i0in, &i0out, &i1in, &i1out) == SUCCESS);
	/*
	 * In the cpp variant, we connect inverters instead of their explicit
	 * interfaces. Here, we use the interfaces so conveniently provided
	 * by inv2.
	 */

	REQUIRE_NOERR(rhdl_connect(i0out, i1in));
	REQUIRE_NOERR(rhdl_connect(i1out, i0in));
	REQUIRE_NOERR(rhdl_connect(s -> connector, i0in));

	return ACCEPT(rhdl_finish_structure(s) == E_NETLIST_CONTAINS_CYCLES);
}

int noSuchInterface()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test10", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	rhdl_connector_t *i = rhdl_component(s, inverter);
	REQUIRE(i);

	REQUIRE( ! rhdl_select(i, "blubb12345doesntexistwhatever"));
	return ACCEPT(rhdl_errno(s) == E_NO_SUCH_INTERFACE);
}

int noSuchEntity()
{
	REQUIRE( ! rhdl_entity(0, "fsdhfjkljshdfklasjshdfkljashkldfjh"));
	return ACCEPT(rhdl_errno() == E_NO_SUCH_ENTITY);
}

int entityExists()
{
	return REJECT(rhdl_begin_structure(0, "Inverter", F_CREATE_STATELESS));
}

int unknownStruct()
{
	long int pad[8];
	rhdl_iface_t *fake = (rhdl_iface_t *) pad;

	REQUIRE( ! rhdl_iface(fake, "irgendwas"));
	return ACCEPT(rhdl_errno() == E_UNKNOWN_STRUCT);
}

int wrongStruct()
{
	rhdl_namespace_t *root = rhdl_namespace(0, 0);
	REQUIRE(root);

	rhdl_connector_t *fake = (rhdl_connector_t *) root;
	REQUIRE( ! rhdl_select(fake, "irgendwas"));
	return ACCEPT(rhdl_errno() == E_WRONG_STRUCT_TYPE);
}

int passThrough()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *sc = s -> connector;
	REQUIRE(sc);

	rhdl_connector_t *in = rhdl_select(sc, "in");
	REQUIRE(in);

	rhdl_connector_t *out = rhdl_select(sc, "out");
	REQUIRE(out);

	return ACCEPT(rhdl_connect(in, out) == E_ILLEGAL_PASSTHROUGH);
}