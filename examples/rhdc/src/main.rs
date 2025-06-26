#![feature(tuple_trait)]
#![feature(unboxed_closures)]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

extern crate regex;
extern crate lazy_static;
extern crate const_format;

mod cstrings;
mod interpreter;
mod stdemerg;
mod wod;
mod console;
mod util;
mod rhdc;
mod librhdl;
mod librhdl_access;
mod resolve;

use crate::console::Outputs;
use crate::console::SimpleConsoleInterpreter;
use crate::interpreter::CommandsBuilder;
use crate::interpreter::CompleterManager;
use crate::interpreter::Processor;
use crate::rhdc::{RHDC, OuterRHDL, InnerRHDL};

use libc;

fn main(){
    let istty = unsafe { libc::isatty(libc::STDIN_FILENO as i32) } != 0;

    if istty {
        println!("RHDConsole, interactive mode");
    }

    let cons = console::Console::new(istty);
    let outputs = Outputs::new(istty);
    let inner_rhdl = InnerRHDL::new(outputs.clone());
    let mut cm = CompleterManager::new();

    let mut cb_outer_rhdl = CommandsBuilder::<OuterRHDL>::new();
    let mut cb_rhdc = CommandsBuilder::<RHDC>::new();
    OuterRHDL::commands(&mut cb_outer_rhdl);
    RHDC::commands(&mut cb_rhdc);
    cb_outer_rhdl.ensure_completers(&mut cm); 
    cb_rhdc.ensure_completers(&mut cm); 
    let p_outer_rhdl = OuterRHDL::new(outputs.clone(), inner_rhdl);
    let outer_rhdl = SimpleConsoleInterpreter::new(p_outer_rhdl, cb_outer_rhdl, &cm);
    let p_rhdc = RHDC::new(outputs, outer_rhdl);
    let rhdc = SimpleConsoleInterpreter::new(p_rhdc, cb_rhdc, &cm);

    cons.run(rhdc);
}


