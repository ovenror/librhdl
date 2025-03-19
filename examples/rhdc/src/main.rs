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

use crate::console::Outputs;
use crate::console::SimpleConsoleInterpreter;

use libc;

fn main(){
    let istty = unsafe { libc::isatty(libc::STDIN_FILENO as i32) } != 0;

    if istty {
        println!("RHDConsole, interactive mode");
    }

    let cons = console::Console::new(istty);
    let rhdc = SimpleConsoleInterpreter::new(rhdc::RHDC::new(Outputs::new(istty)));

    cons.run(rhdc);
}

