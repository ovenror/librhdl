extern crate regex;
extern crate lazy_static;
extern crate const_format;

use crate::librhdl::*;
use crate::util::split_qn;
use crate::interpreter;
use crate::interpreter::Command;
use crate::interpreter::CommandCompleter;
use crate::interpreter::Interpreter;
use crate::cstrings::CStrings;
use crate::console::Outputs;
use crate::console::SimpleConsoleInterpreter;
use crate::console::Commands;

use lazy_static::lazy_static;
use std::str::SplitWhitespace;
use std::str::Split;
use std::io::Write;
use std::process::exit;
use std::ffi::CString;
use std::ffi::CStr;
use std::ptr;
use std::slice;
use std::fmt;
use std::collections::hash_map::HashMap;
use regex::Regex;
use const_format::formatcp;

use rustyline::error::ReadlineError;
use rustyline::Context;
use rustyline::completion::{Completer, Pair};

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

trait Selectable : fmt::Display {
    fn select(&self, name: &str) -> *const Self; 
}

impl Selectable for rhdl_connector_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_connector_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdl_select(this, tname.as_ptr())}  
    }
}

impl Selectable for rhdl_iface_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_iface_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdl_iface(this, tname.as_ptr())}  
    }
}

impl Selectable for rhdl_object_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_object_t = self;
        let tname = CString::new(name).unwrap();
        let selfptr: *const rhdl_object_t = &*self;
        println!("trying to get {} from {} at {:p}", name, unsafe{*self.name}, selfptr);
        unsafe {rhdlo_get(this, tname.as_ptr())}  
    }
}

fn get_interface<S: Selectable>(
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
            write!(err, "{} contains no member named {}", accu, iname).unwrap();
            perror(err);
            return ptr::null_mut();
        }
        
        accu = format!("{}.{}", accu, iname);
    }

    iface
}

fn translate_direction<'a>(dir: rhdl_direction) -> &'a str {
    return match dir {
        rhdl_direction_RHDL_IN => "IN",
        rhdl_direction_RHDL_OUT => "OUT",
        _ => "(error)"
    };
}

impl fmt::Display for rhdl_isingle {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let idir = self.dir;
        let dir = translate_direction(idir); 
        write!(f, "Direction: {}", dir)
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

        write!(f, "{}", CStrings::new(self.members))
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

impl fmt::Display for rhdl_object_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if unsafe{rhdlo_has_value(self)} != 0 {
            write!(f, "{}", unsafe{CStr::from_ptr(rhdlo_read_cstring(self))}.to_str().unwrap())
        }
        else {
            write!(f, "{}", CStrings::new(self.members))
        }
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
            writeln!(self.outputs.out, "successfully instantiated entity {} as component {}", ename, name).unwrap();
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

impl Completer for InnerRHDL {
    type Candidate = Pair;
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
        Command::<Self>("def",Self::define, &OBJECT_COMPLETER),
        Command::<Self>("define",Self::define, &OBJECT_COMPLETER),
        Command::<Self>("stateful",Self::stateful, &OBJECT_COMPLETER),
        Command::<Self>("enddef",Self::enddef, &NO_COMPLETER)
        ];

    fn exec_fb(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str) 
        -> bool
    {
        if !self.rhdd.is_active() {
            return false;
        }

        self.rhdd.exec(command, args, orig)
    }
    
    fn complete_fb(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        if !self.rhdd.is_active() {
            return Ok((pos, Vec::<Pair>::new()))
        }

        return self.rhdd.complete(line, pos, ctx)
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
    
impl<'a> Completer for OuterRHDL {
    type Candidate = Pair;
}

pub struct RHDC<'a> {
    outputs: Outputs,
    rhdl: SimpleConsoleInterpreter<'a, OuterRHDL>
}

impl<'a> RHDC<'a> {
    pub fn new(outputs: Outputs) -> RHDC<'a> {
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

    fn ls_internal<I: Selectable>(&mut self, basename: &str, base: *const I, mut components: &mut Split<char>) -> bool {
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
            let ns = unsafe {rhdlo_get(ptr::null(), ptr::null())};
            println!("{}", unsafe{&*ns});
            return true;
        }

        let (basename, mut components) = split_qn(&name);
        
        let tname = CString::new(basename).unwrap();
        
        let entity = unsafe {rhdl_entity(ptr::null(), tname.as_ptr())};
        if !(entity.is_null()) {
            let iface = unsafe{*entity}.iface;
            return self.ls_internal(basename, iface, &mut components);
        }
        
        let object = unsafe {rhdlo_get(ptr::null(), tname.as_ptr())};
        if !(object.is_null()) {
            //println!("{}", unsafe{*object});
            //return true;
            return self.ls_internal(basename, object, &mut components);
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
                let tname = CString::new(name).unwrap();
                unsafe {ec = rhdl_print_commands(tname.as_ptr());}
                if ec != 0 {
                    write!(self.outputs.err, "{}", name).unwrap();
                    perror(&mut self.outputs.err);
                }
                true
            }
        }
    }
}

impl<'a> Completer for RHDC<'a> {
    type Candidate = Pair;
}

impl<'a> interpreter::Commands<'a> for RHDC<'a> {
    const COMMANDS: &'a [Command<Self>] = &[
        Command::<Self>("quit", Self::quit, &NO_COMPLETER),
        Command::<Self>("panic",Self::panic, &NO_COMPLETER),
        Command::<Self>("ls",Self::ls, &OBJECT_COMPLETER),
        Command::<Self>("synth",Self::synth, &OBJECT_COMPLETER)
        ];
    
    fn exec_fb(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str)
        -> bool 
    {
        self.rhdl.exec(command, args, orig)
    }
    
    fn complete_fb(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        return self.rhdl.complete(line, pos, ctx)
    }
}

impl<'a> Commands<'a> for RHDC<'a> {
    fn prompt_info(&self) -> &str {
        self.rhdl.get_commands().prompt_info()
    }
}

static NO_COMPLETER : NoCompleter = NoCompleter{};
static OBJECT_COMPLETER : ObjectCompleter =  ObjectCompleter{};

struct NoCompleter {}

impl CommandCompleter for NoCompleter {
    fn complete(&self, _text: &str) -> Result<Vec<String>, ()>
    {
        return Err(());
    }
}

struct ObjectCompleter {}

impl CommandCompleter for ObjectCompleter {
    fn complete(&self, text: &str) -> Result<Vec<String>, ()>
    {
        let split = text.rsplit_once('.');

        let (most, last) = match split {
            None => ("", text),
            Some((m, l)) => (m,l)
        };

        let base = resolve_object(most);

        if base.is_null() {
            return Err(());
        }

        let cs = CStrings::new(unsafe{*base}.members);

        if cs.ptr.is_null() {
            panic!("NUUULL");
        }

        let mut sl = unsafe {slice::from_raw_parts(cs.ptr, 1)};
        let mut len = 0;

        while !sl[len].is_null() {
            len = len + 1;
            sl = unsafe {slice::from_raw_parts(cs.ptr, len + 1)};
        }

        sl = unsafe {slice::from_raw_parts(cs.ptr, len)};
        
        let mut vec = Vec::<String>::new();

        for ename in sl {
            match unsafe {CStr::from_ptr(*ename)}.to_str() {
                Ok(name) => {
                    if name.starts_with(&last) {
                        let cand = if most.is_empty() {
                            String::from(name)
                        } else {
                            format!("{}.{}", most, name)
                        };

                        vec.push(cand);
                    }
                }
                Err(_) => {panic!("doood")}
            };
        }

        return Ok(vec); 
    }
}

fn resolve_object(qn : &str) -> *const rhdl_object_t
{
    resolve_object_with_base(ptr::null(), qn)
}

fn resolve_object_with_base(base : *const rhdl_object_t, qn : &str) -> *const rhdl_object_t
{
    if qn.trim().is_empty() {
        return unsafe {rhdlo_get(base, ptr::null())}
    }

    let components = qn.split('.');

    let mut curbase = base;
    let mut component_str : String;

    for component in components {
        component_str = component.trim().to_string();

        let component_cstr = CString::new(component_str).unwrap();
        curbase = unsafe {rhdlo_get(curbase, component_cstr.as_ptr())};

        if curbase.is_null() {
            return ptr::null()
        }
    }

    return curbase
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn failed_resolve() {
        assert!(resolve_object("doesnotexist") == ptr::null());
        assert!(unsafe{rhdl_errno()} == Errorcode_E_NO_SUCH_MEMBER);
    }

    #[test]
    fn root_resolve() {
        assert!(resolve_object("") != ptr::null());
    }

    #[test]
    fn toplevel_resolve() {
        assert!(resolve_object("entities") != ptr::null());
    }

    #[test]
    fn secondlevel_resolve() {
        assert!(resolve_object("entities.Inverter") != ptr::null());
    }

    #[test]
    fn complex_resolve() {
        assert!(resolve_object("entities.AND.representations.AND_Structure_0.content") != ptr::null());
    }

    #[test]
    fn spaced_resolve() {
        assert!(resolve_object("   entities .AND.      representations .   AND_Structure_0. content  ") != ptr::null());
    }

    fn based_resolve() {
        let entities = resolve_object("entities");
        assert!(entities != ptr::null());
        assert!(resolve_object_with_base(entities, "ClockDiv2") != ptr::null());
    }

}
