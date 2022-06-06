#include "misconstruction.h"
#include "rhdl/rhdl.h"
#include "macros.h"

int emptyInterface()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *connector = s -> connector;
	REQUIRE(connector);

	rhdl_iface_t *iface = connector -> iface;
	REQUIRE(iface);
	REQUIRE(iface -> type == RHDL_COMPOSITE);

	const char *const *list = iface -> composite.interfaces;
	REQUIRE(list);
	REQUIRE( ! *list);

	return ACCEPT(rhdl_finish_structure(s) == E_EMPTY_INTERFACE);
}

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

int ambiguousConnection()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test4", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *nand = rhdl_entity(0, "NAND");
	rhdl_connector_t *nand0 = rhdl_component(s, nand);
	REQUIRE(nand0);

	return ACCEPT(rhdl_connect(nand0, nand0) == E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);
}

int statefulComponentInStatelessStructure()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "test6", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *rsl = rhdl_entity(0, "RS_Latch");
	REQUIRE(rhdl_component(s, rsl) == 0);

	return ACCEPT(rhdl_errno() == E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY);
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

	REQUIRE_ERR(rhdl_finish_structure(s), E_NETLIST_CONTAINS_CYCLES);

	return SUCCESS;
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
