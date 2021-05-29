#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
//#![allow(non_snake_case)]

extern crate regex;
extern crate lazy_static;
extern crate const_format;

mod interpreter;
mod stdemerg;
mod wod;
mod console;

use libc;

use lazy_static::lazy_static;
use std::str::SplitWhitespace;
use std::str::Split;
use std::io::Write;
use std::process::exit;
use std::os::raw::c_char;
use std::ffi::CString;
use std::ffi::CStr;
use std::ptr;
use std::slice;
use std::fmt;
use std::collections::hash_map::HashMap;
use regex::Regex;
use const_format::formatcp;

use interpreter::Command;
use interpreter::Interpreter;

use console::Commands;
use console::Outputs;
use console::SimpleConsoleInterpreter;

const ALPHA: &'static str = "A-Za-z";
const IDENTIFIER: &'static str = formatcp!(r"[{0}][{0}0-9_]*", ALPHA);
const IDENTIFIERW: &'static str = formatcp!(r"\s*{}\s*", IDENTIFIER);
const QUALIFIED: &'static str = formatcp!(r"{0}(\.{0})*", IDENTIFIERW);
const OPERATORS: &'static str = r":|->|<-";
const COMPLETE: &'static str = formatcp!(r"^{0}({1}){0}$", QUALIFIED, OPERATORS);


lazy_static! {
    static ref REGEX_ID: Regex = Regex::new(IDENTIFIER).unwrap();
    static ref REGEX_RHDD: Regex = Regex::new(COMPLETE).unwrap();
}

fn split_qn(qn: &str) -> (&str, Split<char>) {
    let mut components = qn.split('.');
    let basename = components.next().unwrap().trim();

    (basename, components)
}

fn errstr() -> &'static str {
    let eptr = unsafe {rhdl_errstr()};
    let ecstr = unsafe {CStr::from_ptr(eptr)};
    
    ecstr.to_str().unwrap()
}

fn perror(err: &mut dyn Write) {
    let ec = unsafe {rhdl_errno()};
    let msg = errstr();
    writeln!(err, ": error #{}: {}", ec, msg).unwrap();
}

trait Interfacible : fmt::Display {
    fn select(&self, name: &str) -> *const Self; 
}

impl Interfacible for rhdl_connector_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_connector_t = self;
        unsafe {rhdl_select(this, CString::new(name).unwrap().as_ptr())}  
    }
}

impl Interfacible for rhdl_iface_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_iface_t = self;
        unsafe {rhdl_iface(this, CString::new(name).unwrap().as_ptr())}  
    }
}

fn get_interface<S: Interfacible>(
    basename: &str, err: &mut dyn Write,
    base: *const S, components: &mut Split<char>
    ) -> *const S
{
    let mut iface = base;
    let mut accu = basename.to_string();
    for component in components {
        let iname = component.trim();

        iface = unsafe {(*iface).select(iname)};

        if iface.is_null() {
            write!(err, "{} contains no interface named {}", accu, iname).unwrap();
            perror(err);
            return ptr::null_mut();
        }
        
        accu = format!("{}.{}", accu, iname);
    }

    iface
}

struct CStrings {
    ptr: *const *const c_char
}

impl CStrings {
    fn new(ptr: *const *const c_char) -> CStrings {
        CStrings {ptr: ptr}
    }
}

impl fmt::Display for CStrings {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result  {
        if self.ptr.is_null() {
            panic!();
        }

        let mut sl = unsafe {slice::from_raw_parts(self.ptr, 1)};
        let mut len = 0;

        while !sl[len].is_null() {
            len = len + 1;
            sl = unsafe {slice::from_raw_parts(self.ptr, len + 1)};
        }

        sl = unsafe {slice::from_raw_parts(self.ptr, len)};

        for ename in sl {
            let result = match unsafe {CStr::from_ptr(*ename)}.to_str() {
                Ok(name) => writeln!(f, "  {}", name),
                Err(_) => return Err(fmt::Error)
            };

            if let Err(fmt::Error) = result {
                return Err(fmt::Error);
            }
        }

        Ok(())
    }
}

impl fmt::Display for rhdl_isingle {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let idir = self.dir;
        let iopen = self.open;

        let dir = match idir {
            rhdl_direction_RHDL_IN => "IN",
            rhdl_direction_RHDL_OUT => "OUT",
            _ => "(error)"
        };

        let open = match iopen {
            1 => "YES",
            0 => "NO",
            _ => "(error)"
        };

        write!(f, "Direction: {}, open: {}", dir, open)
    }
}

impl fmt::Display for rhdl_icomposite {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", CStrings::new(self.interfaces))
    }
}

impl fmt::Display for rhdl_namespace_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if let Err(fmt::Error) = writeln!(f, "Entities:") {
            return Err(fmt::Error);
        }

        write!(f, "{}", CStrings::new(self.entities))
    }
}

impl fmt::Display for rhdl_entity_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", &unsafe {*self.iface})
    }
}

impl fmt::Display for rhdl_iface_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let u = self.__bindgen_anon_1;
        match self.type_ {
            rhdl_iface_type_RHDL_SINGLE => write!(f, "{}", unsafe {u.single}),
            rhdl_iface_type_RHDL_COMPOSITE => write!(f, "{}", unsafe {u.composite}),
            rhdl_iface_type_RHDL_UNSPECIFIED => write!(f, "(unspecified)"),
            _ => write!(f, "(error)")
        }
    }
}

impl fmt::Display for rhdl_connector_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.iface.is_null() {
            panic!()
        }

        write!(f, "{}", &unsafe {*self.iface})
    }
}

struct InnerRHDL {
    outputs: Outputs,
    active: bool,
    ename: String,
    structure: *const rhdl_structure_t,
    components: HashMap<String, *const rhdl_connector_t>
}

impl InnerRHDL {
    fn new(outputs: Outputs) -> InnerRHDL {
        InnerRHDL {
            outputs: outputs,
            active: false,
            ename: String::from(""),
            structure: ptr::null(),
            components: HashMap::new()
        }
    }

    fn define(&mut self, ename: &str, stateless: bool) -> bool {
        if self.active {
            panic!("already defining {}", self.ename);
        }

        if ename == "" {
            panic!("structure name cannot be empty");
        }

        let cename = CString::new(ename).unwrap();
   
        let mode = if stateless {Flags_F_CREATE_STATELESS} else {Flags_F_CREATE_STATEFUL};

        unsafe {    
            self.structure = rhdl_begin_structure(ptr::null(), cename.as_ptr(), mode);
        }

        if self.structure.is_null() {
            write!(self.outputs.err, "cannot define entity {}", self.ename).unwrap();
            self.perror();
            false
        }
        else {
            writeln!(self.outputs.out, "defining structure for entity {}", self.ename).unwrap();
            self.active = true;
            self.ename = ename.to_string();
            true
        }
    }

    fn assert_active(&self) {
        if !self.active {
            panic!("not defining anything currently");
        }
    }

    fn enddef(&mut self) {
        self.assert_active();
        self.active = false;
        self.components = HashMap::new();
        let ec;

        unsafe {
            ec = rhdl_finish_structure(self.structure);
        }

        if ec == 0 {
            writeln!(self.outputs.out, "successfully defined structure for {}", self.ename).unwrap();
        }
        else {
            write!(self.outputs.err, "finalizing structure {}", self.ename).unwrap();
            self.perror();
        }
    }

    fn is_active(&self) -> bool {
        self.active
    }

    fn get_ename(&self) -> &str {
        self.assert_active();
        &self.ename
    }

    fn get_connectible(&mut self, name: &str) -> *const rhdl_connector_t {
        let (basename, mut components) = split_qn(name);
        let base = self.get_interfacible(basename);

        if base.is_null() {
            writeln!(self.outputs.err, "{} is neither the definee nor one of its components", basename).unwrap();
            return ptr::null_mut(); 
        }

        get_interface(basename, &mut self.outputs.err, base, &mut components)
    }
    
    fn get_interfacible(&self, name: &str) -> *const rhdl_connector_t {
        if name == self.ename {
            return unsafe {(*self.structure).connector};
        }
        
        match self.components.get(name) {
            Some(component) => *component,
            None => ptr::null_mut()
        }
    }

    fn perror(&mut self) {
        perror(&mut self.outputs.err)
    }

    fn instantiate(&mut self, name: &str, ename: &str) {
        let cename = CString::new(ename).unwrap();

        let entity = unsafe {rhdl_entity(ptr::null(), cename.as_ptr())};

        let component = if entity.is_null() {
            ptr::null()
        } else {
            unsafe {rhdl_component(self.structure, entity)}
        };

        if component.is_null() {
            write!(self.outputs.err, "instantiating entity {}", ename).unwrap();
            self.perror();
            return;
        }
        else {
            writeln!(self.outputs.out, "sucessfully instantiated entity {} as component {}", ename, name).unwrap();
        }
        
        self.components.insert(name.to_string(), component);
    }

    fn connect(&mut self, id1: &str, id2: &str) {
        let ptr1 = self.get_connectible(id1);
        let ptr2 = self.get_connectible(id2);

        if ptr1.is_null() || ptr2.is_null() {
            return;
        }

        let ec;

        unsafe {
            ec = rhdl_connect(ptr1, ptr2);
        }

        if ec == 0 {
            writeln!(self.outputs.out, "successfully connected {} -> {}", id1, id2).unwrap();
        }
        else {
            write!(self.outputs.err, "connecting {} -> {}", id1, id2).unwrap();
            self.perror();
        }
    }
}

impl interpreter::Interpreter for InnerRHDL {
    fn exec(self : &mut Self, _command: &str, _args: &mut SplitWhitespace, orig: &str) 
        -> bool
    {
        let cap = match REGEX_RHDD.captures(orig) {
            Some(v) => v,
            None => return false
        };
        
        self.assert_active();

        //dbg!("{}", &cap);

        let mtch = cap.get(2).unwrap();
        let (before, remainder) = orig.split_at(mtch.start());
        let id1 = before.trim();
       
        match mtch.as_str() {
            "->" => self.connect(id1, remainder[2..].trim()),
            "<-" => self.connect(remainder[2..].trim(), id1),
            ":" => self.instantiate(id1, remainder[1..].trim()),
            _ => panic!()
        }

        return true;
    }
}

struct OuterRHDL {
    outputs: Outputs,
    rhdd: InnerRHDL
}

impl OuterRHDL {
    fn new(outputs: Outputs) -> OuterRHDL {
        let oc = outputs.clone();

        OuterRHDL {
            outputs: outputs,
            rhdd: InnerRHDL::new(oc)
        }
    }
    
    fn define(&mut self, args: &mut SplitWhitespace) -> bool {
        self.define_internal(args, true)
    }

    fn stateful(&mut self, args: &mut SplitWhitespace) -> bool {
        self.define_internal(args, false)
    }

    fn define_internal(&mut self, args: &mut SplitWhitespace, stateless: bool) -> bool {
        if self.rhdd.is_active() {
            writeln!(self.outputs.err, "Already defining {}", self.rhdd.get_ename()).unwrap();
            return true;
        }

        let ename = match args.next() {
            Some(s) => s,
            None => {
                writeln!(self.outputs.err, "usage: define <name>").unwrap();
                return true;
            }
        };

        self.rhdd.define(ename, stateless);
        return true;
    }

    fn enddef(&mut self, _ : &mut SplitWhitespace) -> bool {
        if self.rhdd.is_active() {
            self.rhdd.enddef();
        }
        else {
            writeln!(self.outputs.err, "not currently defining a structure").unwrap();
        }

        return true;
    }
    
    fn get_interfacible(&self, name: &str) -> std::result::Result<*const rhdl_connector_t, ()> {
        if self.rhdd.is_active() {
            Ok(self.rhdd.get_interfacible(name))
        }
        else {
            Err(())
        }
    }

}

impl<'a> interpreter::Commands<'a> for OuterRHDL {
    const COMMANDS: &'a [Command<Self>] = &[
        Command::<Self>("def",Self::define),
        Command::<Self>("define",Self::define),
        Command::<Self>("stateful",Self::stateful),
        Command::<Self>("enddef",Self::enddef)
        ];

    fn exec_fb(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str) 
        -> bool
    {
        if !self.rhdd.is_active() {
            return false;
        }

        self.rhdd.exec(command, args, orig)
    }
}

impl<'a> Commands<'a> for OuterRHDL {
    fn prompt_info(&self) -> &str {
        match self.rhdd.active {
            true => &self.rhdd.get_ename(),
            false => ""
        }
    }
}

struct RHDC<'a> {
    outputs: Outputs,
    rhdl: SimpleConsoleInterpreter<'a, OuterRHDL>
}

impl<'a> RHDC<'a> {
    fn new(outputs: Outputs) -> RHDC<'a> {
        let oc = outputs.clone();

        RHDC {
            outputs: outputs,
            rhdl: SimpleConsoleInterpreter::new(OuterRHDL::new(oc))
        }
    }

    fn quit(&mut self, _ : &mut SplitWhitespace) -> bool {
        writeln!(self.outputs.out, "Quitting.").unwrap();
        exit(0);
    }

    fn panic(&mut self, _ : &mut SplitWhitespace) -> bool {
        writeln!(self.outputs.out, "ok panic").unwrap();
        panic!();
    }

    fn ls_internal<I: Interfacible>(&mut self, basename: &str, base: *const I, mut components: &mut Split<char>) -> bool {
        let iface = get_interface(basename, &mut self.outputs.err, base, &mut components);

        if iface.is_null() {
            return true;
        }
        println!("{}", unsafe{&*iface});

        true
    }

    fn ls(&mut self, args : &mut SplitWhitespace) -> bool {
        let name = args.fold(String::from(""), |acc, arg| {acc + arg});

        if name == "" {
            let ns = unsafe {rhdl_namespace(ptr::null(), ptr::null())};
            println!("{}", &unsafe{*ns});
            return true;
        }

        let (basename, mut components) = split_qn(&name);
        
        let entity = unsafe {rhdl_entity(ptr::null(), CString::new(basename).unwrap().as_ptr())};
        if !(entity.is_null()) {
            let iface = unsafe{*entity}.iface;
            return self.ls_internal(basename, iface, &mut components);
        }

        let connector = match self.rhdl.get_commands().get_interfacible(basename) {
            Ok(ptr) => ptr,
            Err(_) => {
                write!(self.outputs.err, "Unknown entity {}", basename).unwrap();
                perror(&mut self.outputs.err);
                return true;
            }
        };

        if connector.is_null() {
            writeln!(self.outputs.err, "{} is neither a known entity, nor an identifier used in the current structure definition", basename).unwrap();
            return true;
        }
            
        return self.ls_internal(basename, connector, &mut components); 
    }

    fn synth(&mut self, args: &mut SplitWhitespace) -> bool {
        match args.next() {
            None => {
                writeln!(self.outputs.err, "usage: synth <entity name>").unwrap();
                true
            },
            Some(name) => {
                let ec;
                unsafe {ec = rhdl_print_commands(CString::new(name).unwrap().as_ptr());}
                if ec != 0 {
                    write!(self.outputs.err, "{}", name).unwrap();
                    perror(&mut self.outputs.err);
                }
                true
            }
        }
    }
}

impl<'a> interpreter::Commands<'a> for RHDC<'a> {
    const COMMANDS: &'a [Command<Self>] = &[
        Command::<Self>("quit", Self::quit),
        Command::<Self>("panic",Self::panic),
        Command::<Self>("ls",Self::ls),
        Command::<Self>("synth",Self::synth)
        ];
    
    fn exec_fb(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str)
        -> bool 
    {
        self.rhdl.exec(command, args, orig)
    }
}

impl<'a> Commands<'a> for RHDC<'a> {
    fn prompt_info(&self) -> &str {
        self.rhdl.get_commands().prompt_info()
    }
}

fn main(){
    let istty = unsafe { libc::isatty(libc::STDIN_FILENO as i32) } != 0;

    if istty {
        println!("RHDConsole, interactive mode");
    }

    let cons = console::Console::new(istty);
    let mut rhdc = SimpleConsoleInterpreter::new(RHDC::new(Outputs::new(istty)));

    cons.run(&mut rhdc);
}

