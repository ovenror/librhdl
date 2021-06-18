#include "lib.h"
#include "construction/library.h"
#include "construction/buildsim.h"

using namespace rhdl;

void lib_init()
{

{
	Ent nor("NOR");
	Part inv("Inverter");

	nor["in0"] >> inv;
	nor["in1"] >> inv;
	inv >> nor["out"];

	using iface = INTERFACE(SINGLE(in0), SINGLE(in1), SINGLE(out));

	nor.addTimedBehavior(
		FASTSIMFUNC(iface, NO_STATE) {
			SIMGET(out) = !(SIMGET(in0) || SIMGET(in1));
			SIMCLR(in0);
			SIMCLR(in1);
		},
		FASTSIMFUNC(iface, NO_STATE) {
			bool on = SIMGET(in0) || SIMGET(in1);
			SIMGET(in0) = on;
			SIMGET(in1) = on;
		},
		{}
	);

	nor.addFunctionalBehavior(
		FUNCTION(iface) {
			SIMGET(out) = !(SIMGET(in0) || SIMGET(in1));
			SIMCLR(in0);
			SIMCLR(in1);
		}
	);
}

{
	Ent And("AND");
	Part inv0("Inverter");
	Part inv1("Inverter");
	Part inv_out("Inverter");

	And["in0"] >> inv0 >> inv_out;
	And["in1"] >> inv1 >> inv_out;
	And["out"] << inv_out;

	using iface = INTERFACE(SINGLE(in0), SINGLE(in1), SINGLE(out));

	And.addTimedBehavior(
		FASTSIMFUNC(iface, NO_STATE) {
			SIMGET(out) = SIMGET(in0)[1] && SIMGET(in1)[1];
			SIMCLR(in0);
			SIMCLR(in1);
		},
		FASTSIMFUNC(iface, NO_STATE) {SIMNOP;},
		{{"in0", {true}}, {"in1", {true}}}
	);

	And.addFunctionalBehavior(
		FUNCTION(iface) {
			SIMGET(out) = SIMGET(in0) && SIMGET(in1);
			SIMCLR(in0);
			SIMCLR(in1);
		}
	);
}

{
	Ent nand("NAND");

	Part inv0("Inverter");
	Part inv1("Inverter");

	nand["in"]["bit0"] >> inv0;
	nand["in"]["bit1"] >> inv1;
	inv0 >> nand["out"];
	inv1 >> nand["out"];

	using iface =
		INTERFACE(
			COMPOSITE(in,
				SINGLE(bit0),
				SINGLE(bit1)),
			SINGLE(out));


	nand.addTimedBehavior(FASTSIMFUNC(iface, NO_STATE)
	{
		SIMGET(out) = !(SIMGET(in.bit0) && SIMGET(in.bit1));
		SIMCLR(in.bit0);
		SIMCLR(in.bit1);
	});

	nand.addFunctionalBehavior(FUNCTION(iface)
	{
		SIMGET(out) = !(SIMGET(in.bit0) && SIMGET(in.bit1));
		SIMCLR(in.bit0);
		SIMCLR(in.bit1);
	});
}

{
	Ent rslatch("RS_Latch", Ent::CREATE_STATEFUL);

	Part invQ("Inverter");
	Part invNQ("Inverter");

	rslatch["S"] >> invNQ >> invQ >> rslatch["Q"];
	rslatch["R"] >> invQ >> invNQ >> rslatch["NQ"];

	using iface = INTERFACE(SINGLE(S), SINGLE(Q), SINGLE(R), SINGLE(NQ));

	rslatch.addTimedBehavior(
		FASTSIMFUNC(iface, NO_STATE)
		{
			bool q = true;
			bool nq = true;

			bool set_q = SIMGET(S) || SIMGET(Q);
			bool set_nq = SIMGET(R) || SIMGET(NQ);

			if (set_q)
				nq = false;

			if (set_nq)
				q = false;

			SIMGET(S) = q;
			SIMGET(Q) = q;
			SIMGET(R) = nq;
			SIMGET(NQ) = nq;
		},
		FASTSIMFUNC(iface, NO_STATE)
		{
			SIMGET(S) = (SIMGET(Q) = SIMGET(Q) || SIMGET(S));
			SIMGET(R) = (SIMGET(NQ) = SIMGET(NQ) || SIMGET(R));
		}
	);
}

{
	Ent crslatch("GRS_Latch", Ent::CREATE_STATEFUL);
	Part rs("RS_Latch");
	Part r_gate("AND");
	Part s_gate("AND");

	crslatch["R"] >> r_gate["in0"];
	crslatch["S"] >> s_gate["in0"];
	crslatch["C"] >> r_gate["in1"];
	crslatch["C"] >> s_gate["in1"];

	crslatch["Q"] << rs["Q"];
	crslatch["NQ"] << rs["NQ"];

	r_gate >> rs["R"];
	s_gate >> rs["S"];

	using iface = INTERFACE(
		SINGLE(R), SINGLE(S), SINGLE(C), SINGLE(Q), SINGLE(NQ));

	crslatch.addTimedBehavior(
		FASTSIMFUNC(iface, NO_STATE)
		{
			bool q = !SIMGET(NQ) || (SIMGET(S)[1] && SIMGET(C)[1]);
			bool nq = !SIMGET(Q) || (SIMGET(R)[1] && SIMGET(C)[1]);

			SIMGET(Q) = q;
			SIMGET(NQ) = nq;

			SIMCLR(R);
			SIMCLR(S);
			SIMCLR(C);
		},
		FASTSIMFUNC(iface, NO_STATE)
		{
			SIMNOP;
		},
		{
			{"R", {false}},
			{"S", {false}},
			{"C", {false}}
		}
	);
}

{
	Ent ff("D_Flipflop", Ent::CREATE_STATEFUL);

	Part master("GRS_Latch");
	Part slave("GRS_Latch");
	Part cinv("Inverter");
	Part dinv("Inverter");

	ff["D"] >> master["S"];
	ff["D"] >> dinv >> master["R"];

	ff["clk"] >> cinv >> master["C"];
	ff["clk"] >> slave["C"];

	master["NQ"] >> slave["R"];
	master["Q"] >> slave["S"];

	slave["Q"] >> ff["Q"];
	slave["NQ"] >> ff["NQ"];
}

{
	Ent clock3("Clock3", Ent::CREATE_STATEFUL);
	Part inv0("Inverter");
	Part inv1("Inverter");
	Part inv2("Inverter");

	inv0 >> inv1;
	inv1 >> inv2;
	inv2 >> inv0;

	inv2 >> clock3["out"];

	using iface = INTERFACE(SINGLE(out));

	struct cstate {
		cstate() : sync(true), phase(0) {}
		bool sync;
		int phase;
	};

	clock3.addTimedBehavior(FASTSIMFUNC(iface, cstate)
	{
		if (SIMINT.sync) {
			if (SIMGET(out) && (SIMINT.phase == 0)) {
				SIMINT.phase = 4;
				SIMINT.sync = false;
			}
		}
		else {
			if (SIMGET(out) && (SIMINT.phase < 3)) {
				switch (SIMINT.phase) {
				case 0: SIMINT.phase = 5;
						break;
				case 2: SIMINT.phase = 3;
						break;
				case 1: SIMINT.sync = true;
						SIMINT.phase = 1;
				}
			}
		}

		if (SIMINT.sync) {
			SIMINT.phase = (SIMINT.phase+1)%2;
			SIMGET(out) = SIMINT.phase != 0;
		}
		else {
			SIMINT.phase = (SIMINT.phase+1)%6;
			SIMGET(out) = SIMINT.phase >= 3;
		}
	});
}

{
	Ent p1("Pulse1");

	Part outinv("Inverter");
	Part shutter1("Inverter");
	Part shutter2("Inverter");
	Part ininv("Inverter");

	p1["in"] >> ininv;
	ininv >> outinv;
	outinv >> p1["out"];

	p1["in"] >> shutter1;
	shutter1 >> shutter2;
	shutter2 >> outinv;
}

{
	Ent dm2("Demul2");

	Part outinv0("Inverter");
	Part outinv1("Inverter");
	Part ininv0("Inverter");
	Part ininv1("Inverter");
	Part selinv("Inverter");

	dm2["in"] >> ininv0;
	dm2["in"] >> ininv1;

	ininv0 >> outinv0;
	ininv1 >> outinv1;

	dm2["sel"] >> outinv0;
	dm2["sel"] >> selinv;
	selinv >> outinv1;

	outinv0 >> dm2["out0"];
	outinv1 >> dm2["out1"];

	using iface = INTERFACE(SINGLE(in), SINGLE(sel), SINGLE(out0), SINGLE(out1));

	dm2.addFunctionalBehavior(FUNCTION(iface)
	{
		SIMGET(out0) = SIMGET(in) && !SIMGET(sel);
		SIMGET(out1) = SIMGET(in) && SIMGET(sel);

		SIMGET(sel) = !SIMGET(in);
		SIMCLR(in);
	});
}

{
	Ent cdiv("ClockDiv2", Ent::CREATE_STATEFUL);

	Part master("RS_Latch");
	Part slave("GRS_Latch");
	Part demul("Demul2");
	Part clockinv("Inverter");
	Part dirinv("Inverter");
	Part qInv("Inverter");
	Part nqInv("Inverter");

	cdiv["clk"] >> clockinv;
	clockinv >> demul["in"];

	cdiv["reset"] >> demul["sel"];
	demul["out0"] >> master["R"];
	demul["out1"] >> master["S"];

	slave["Q"] >> dirinv;
	dirinv >> demul["sel"];

	cdiv["clk"] >> slave["C"];

	master["Q"] >> slave["S"];
	master["NQ"] >> slave["R"];

	slave["Q"] >> cdiv["out"];
}

}

