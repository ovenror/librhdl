#include "construction.h"
#include "rhdl/rhdl.h"
#include "macros.h"
#include "strlist.h"

int instantiateStructure()
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

	return REJECT_ERR(rhdl_abort_structure(s));
}

int getEntityInterface()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test2", F_CREATE_STATELESS);
	REQUIRE(s);

	return ACCEPT(rhdl_select(s -> connector, "foo"));
}


/*
 * no int instantiateInverter():
 *
 * Instantiating Components independently of a Structure definition is not
 * possible with the C interface.
 */

int exposeInverter()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test 3", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *sc = s -> connector;
	REQUIRE(sc);

	rhdl_iface_t *si = sc -> iface;
	REQUIRE(si);
	REQUIRE(si -> type == RHDL_COMPOSITE);
	REQUIRE(si -> composite.interfaces);

	const char *const *const *sifaces = &si -> composite.interfaces;
	REQUIRE(*sifaces);
	REQUIRE( ! **sifaces);

	const char *const *list;

	rhdl_connector_t *s_in = rhdl_select(sc, "in");
	REQUIRE(s_in);
	rhdl_iface_t *i_in = s_in -> iface;
	REQUIRE(i_in);
	REQUIRE(i_in -> type == RHDL_UNSPECIFIED);

	list = *sifaces;
	REQUIRE(list);
	CHECK(&list, "in");
	END(list);

	rhdl_connector_t *s_out = rhdl_select(sc, "out");
	REQUIRE(s_out);
	rhdl_iface_t *i_out = s_out -> iface;
	REQUIRE(i_out);
	REQUIRE(i_out -> type == RHDL_UNSPECIFIED);

	list = *sifaces;
	REQUIRE(list);
	CHECK(&list, "in");
	CHECK(&list, "out");
	END(list);

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	rhdl_connector_t *inv = rhdl_component(s, inverter);
	REQUIRE(inv);

	REQUIRE_NOERR(rhdl_connect(s_in, inv));
	i_in = s_in -> iface;
	REQUIRE(i_in -> type == RHDL_SINGLE);
	REQUIRE(i_in -> single.dir == RHDL_IN);

	REQUIRE_NOERR(rhdl_connect(inv, s_out));
	i_out = s_out -> iface;
	REQUIRE(i_out -> type == RHDL_SINGLE);
	REQUIRE(i_out -> single.dir == RHDL_OUT);

	return SUCCESS;
}

/*
 * no int connectInverters(), same reason as for instantiateInverter 
 */

int unnamedExposure()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test4", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *sc = s -> connector;
	REQUIRE(sc);

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	rhdl_connector_t *inv = rhdl_component(s, inverter);
	REQUIRE(inv);

	REQUIRE_NOERR(rhdl_connect(inv, sc));

	rhdl_iface_t *si = sc -> iface;
	REQUIRE(si);
	REQUIRE(si -> type == RHDL_COMPOSITE);

	const char *const *list = si -> composite.interfaces;
	REQUIRE(list);
	CHECK(&list, "out");
	END(list);

	return SUCCESS;
}

/*
 * Nevertheless, connecting 2 components should be tested here, albeit
 * with slightly different semantics, since we have to begin a structure
 * definition first.
 */
int connectInverters2()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test5", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	rhdl_connector_t *inv0 = rhdl_component(s, inverter);
	rhdl_connector_t *inv1 = rhdl_component(s, inverter);
	REQUIRE(inv0 && inv1);

	return REJECT_ERR(rhdl_connect(inv0, inv1));
}

int exposeComposite()
{
	rhdl_structure_t *s = rhdl_begin_structure(0, "Test6", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *sc = s -> connector;
	REQUIRE(sc);

	rhdl_connector_t *s_in = rhdl_select(sc, "in");
	REQUIRE(s_in);

	rhdl_entity_t *enand = rhdl_entity(0, "NAND");
	REQUIRE(enand);

	rhdl_connector_t *nand = rhdl_component(s, enand);
	REQUIRE(nand);

	REQUIRE_NOERR(rhdl_connect(s_in, nand));

	rhdl_iface_t *i_in = s_in -> iface;
	REQUIRE(i_in);
	REQUIRE(i_in -> type == RHDL_COMPOSITE);

	const char *const *list = i_in -> composite.interfaces;
	CHECK(&list, "bit0");
	CHECK(&list, "bit1");
	END(list);

	return SUCCESS;
}
