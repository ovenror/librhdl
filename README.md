# librhdl
is a C++ "library" for hardware synthesis. Unfortunately, "hardware" means Redstone wires and torches in Minecraft. And also repeaters, because of the wire length limit.

In the unlikely case that you are actually interested in using the library for
your own project, open a discussion item. Otherwise you might be more
interested in **rhdc**, the console application intended to test and play
around with librhdl, including synthesizing redstone hardware.

![Creating a 4-bit counter with rhdc.](../assets/rhdc.png)

\*The clock on the bottom and the display on top were created manually. The 4-bit-counter in between was created by executing a Minecraft *function* consisting of *setblock* commands generated by rhdc. For a detailed how-to, read on. 

And yes, others build complete CPUs in that size, I know. Currently, *everything* is first broken down to a directed graph (graphs are nice), which is then synthesized: edges to inverters, vertices to wiring. No pistons.

## Building

### librhdl

You'll need:
* cmake 3.13
* C++-Compiler capable of C++17
* boost 1.71

If you want to build the tests
* Google Test
* pthreads

On UNIX-like systems, it should suffice to run `cmake .` and then `make` in the project root directory (I don't know about Windows lol). When that is done, you can move on to building

### rhdc

You'll need:
* Rust 2018

Run "make rhdc" in the same directory as above. The directory `bin/` should now contain the executable `rhdc`.

## Using rhdc

* `> ls` shows a list of available entities. The `Inverter` is the basis for everything, the others are hackishly created on library startup and not adhering to some kind of common naming scheme etc. As they mainly serve a testing purpose, they might be changed or removed, so don't rely on them too much.
* `> ls D_Flipflop` lists the d-flip-flops interfaces
* `> ls D_Flipflop.clk` displays the properties of the clock interface. The library forbids connecting an open output to an open input. An interface is "open" if
  * it is an output, but powering it would affect the entities circuit
  * it is an input that might be powered by the entitiy itself

### Defining an Entity with Combinational Logic

Let's define a XOR gate:

    > def XOR

This starts the structural definition (we do not have behavioral ones for now) of a purely combinational (or functional) circuit. The basic building blocks are only inverters and an implicit OR (via connecting 2 interfaces to the same counterpart). But that is enough to emulate every logic gate. Fortunately there is already an AND, so the formula for XOR would look like `(a AND !b) OR (!a AND b)`. Unfortunately, we cannot (yet) write it directly like that.

Let's focus on the first conjunction, for which we need an `AND` and an Inverter.

    XOR> a0: AND
    sucessfully instantiated entity AND as component a0
    XOR> i0: Inverter
    sucessfully instantiated entity Inverter as component i0

Since we need to invert b:

    XOR> XOR.b -> i0
    successfully connected XOR.b -> i0

Now, we want to connect both a and i0 (the inverted b) to a0 (the first AND). However we should first take a look at a0s interface:

    XOR> ls a0
      in0
      in1
      out

    XOR> ls a0.in0
    Direction: IN, open: NO
    XOR> ls a0.in1
    Direction: IN, open: NO
    XOR> ls a0.out
    Direction: OUT, open: NO

Let's hope it does what it seems to do:

    XOR> XOR.a -> a0.in0
    successfully connected XOR.a -> a0.in0
    XOR> i0 -> a0.in1
    successfully connected i0 -> a0.in1

The first conjunction is done. The second one goes likewise:

    XOR> a1: AND
    sucessfully instantiated entity AND as component a1
    XOR> i1: Inverter
    sucessfully instantiated entity Inverter as component i1
    XOR> XOR.a -> i1
    successfully connected XOR.a -> i1
    XOR> i1 -> a1.in0
    successfully connected i1 -> a1.in0
    XOR> XOR.b -> a1.in1
    successfully connected XOR.b -> a1.in1
    
Second conjunction: done. We now need to OR both conjunctions. In Redstone, this simply means connecting wires. In librhdl, this means connecting two outputs to the same interface:

    XOR> a0.out -> XOR.out
    successfully connected a0.out -> XOR.out
    XOR> a1.out -> XOR.out
    successfully connected a1.out -> XOR.out
    
Done. We can finish the definition:

    XOR> enddef
    successfully defined structure for XOR

That's it. We can now use `XOR` for other structure definitions or we can generate redstone "hardware".

### "Hardware" synthesis

    > synth XOR
    setblock 0 0 0 minecraft:air replace
    setblock 0 0 1 minecraft:quartz_block replace
    setblock 0 0 2 minecraft:air replace
    setblock 0 0 3 minecraft:quartz_block replace
    setblock 0 0 4 minecraft:quartz_block replace
    ...
    
What you see here is a list of /setblock commands for the Minecraft server console. This way it is of course not very useful. However, instead of typing your commands into the interactive rhdc, you can also write them into a file, and pipe that file to rhdc. Fortunately, that file already exists:

    bin/rhdc < rhdc_examples/xor.rhdc

You can use its output as a *function* in Minecraft by saving it to `<saved minecraft world>/datapacks/<datapack>/data/<namespace>/functions/<name>.mcfunction`. For example:

    bin/rhdc < rhdc_examples/xor.rhdc > .minecraft/saves/my_super_creative_game/datapacks/rhdl/data/rhdl/functions/xor.mcfunction
    
To use the "datapack" in minecraft, there has to be a `pack.mcmeta` in `.minecraft/saves/my_super_creative_game/datapacks/rhdl/` which may look like this:

    {
      "pack": {
        "pack_format": 7,
        "description": "whatever"
      }
    }
    
In Minecraft, you can execute the function in the console via

  `/function rhdl:xor`
  
Plop. If you don't see anything, that is because the circuit is placed at (0,0,0), very much at the bottom. If you don't want to dig, use "superflat" as world type. Or issue a feature request :o)

Anyway, in a superflat world it looks like the image below. Not the most efficient design, but further (lol) optimization is part of the plan.

![Synthesized XOR in superflat Minecraft world](../assets/xor.png)

### Sequential Circuits: Latches and Clocks

The library will complain, if you try to add stateful (sequential) components like latches to a combinatorial entity. It also complains when it detects a cycle in the resulting circuit, which could either be a latch or a clock. If you do want to create a stateful entity use `stateful` instead of `def`.

The 4-bit counter in the image at the top is a perfect example. It is also located in `rhdc_examples`.

**Be advised that structurally defined sequential logic is inherently unstable, especially when it is timing-critical, due to the repeaters that may have to be inserted.**

The standard 3-tick clock (`Clock3`) is probably never suited as a clock generator. For the above example it provenly isn't, but a series of 3 fully cranked-up repeaters and an inverter with its output looped back to the first repeater (half-period of 12 redstone ticks) should work fine.

For slow clocks, you might also want to look [here](https://minecraft.fandom.com/wiki/Redstone_circuits/Clock) (Don't use `ClockDiv2`, it does not work). However, librhdl cannot (yet) synthesize these clocks, as they use more than just wires and torches.

That's about it. Have fun! 😃
