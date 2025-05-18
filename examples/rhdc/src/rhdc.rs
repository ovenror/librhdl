extern crate regex;
extern crate lazy_static;
extern crate const_format;

use crate::interpreter;
use crate::interpreter::Command;
use crate::interpreter::CommandCompleter;
use crate::interpreter::Interpreter;
use crate::console::Outputs;
use crate::console::SimpleConsoleInterpreter;
use crate::console::Commands;
use crate::util::split_qn_once;
use crate::librhdl::*;
use crate::resolve::*;
use crate::librhdl_access::*;

use lazy_static::lazy_static;
use std::str::SplitWhitespace;
use std::io::Write;
use std::process::exit;
use std::ffi::CString;
use std::ptr;
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
const OPERATOR: &'static str = r":|->?|<-?";
const COMPLETE: &'static str = formatcp!(r"^({0})?(({1})(({0}))?)?$", QUALIFIED, OPERATOR);


lazy_static! {
    //static ref REGEX_ID: Regex = Regex::new(IDENTIFIER).unwrap();
    static ref REGEX_RHDD: Regex = Regex::new(COMPLETE).unwrap();
}

struct InnerRHDLParseResult<'a> {
    parsed: u8,
    id1: Option<&'a str>,
    operator: Option<&'a str>,
    id2: Option<&'a str>
}

impl<'a> InnerRHDLParseResult<'a> {
    fn new(
        parsed: u8, id1: Option<&'a str>, operator: Option<&'a str>,
        id2: Option<&'a str>) -> InnerRHDLParseResult<'a>
    {
        InnerRHDLParseResult {
            parsed: parsed,
            id1: id1,
            operator: operator,
            id2: id2
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

    fn parse(cmd: &str) -> InnerRHDLParseResult {
        let cap = match REGEX_RHDD.captures(cmd) {
            Some(v) => v,
            None => return InnerRHDLParseResult::new(0, None, None, None)
        };

        let id1 = Some(match cap.get(1) {
            None => return InnerRHDLParseResult::new(0, None, None, None),
            Some(c) => c.as_str().trim()
        });

        let operator = Some(match cap.get(4) {
            None => return InnerRHDLParseResult::new(1, id1, None, None),
            Some(c) => c.as_str()
        });

        let id2 = Some(match cap.get(6) {
            None => return InnerRHDLParseResult::new(2, id1, operator, None),
            Some(c) => c.as_str().trim()
        });

        return InnerRHDLParseResult::new(3, id1, operator, id2)
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
        let (basename, components) = split_qn_once(name);
        let base = self.get_connector(basename);

        if base.is_null() {
            writeln!(self.outputs.err, "{} is neither the definee nor one of its components", basename).unwrap();
            return ptr::null_mut(); 
        }

        resolve_with_base_err(base, components, basename, &mut self.outputs.err)
    }
    
    fn get_connector(&self, name: &str) -> *const rhdl_connector_t {
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
        self.assert_active();

        let parsed = Self::parse(orig);

        if parsed.parsed != 3 {
            return false;
        }

        let id1 = parsed.id1.unwrap();
        let id2 = parsed.id2.unwrap();

        match parsed.operator.unwrap() {
            "->" => self.connect(id1, id2),
            "<-" => self.connect(id2, id1),
            ":" => self.instantiate(id1, id2),
            _ => return false
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
    
    fn get_connector(&self, name: &str) -> std::result::Result<*const rhdl_connector_t, ()> {
        if self.rhdd.is_active() {
            Ok(self.rhdd.get_connector(name))
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

    fn ls_internal<I: Selectable>(&mut self, basename: &str, base: *const I, qn: &str) -> bool {
        let resolved = resolve_with_base_err(base, qn, basename, &mut self.outputs.err);

        if resolved.is_null() {
            return true;
        }
        println!("{}", unsafe{&*resolved});

        true
    }
   
    fn ls(&mut self, args : &mut SplitWhitespace) -> bool {
        let name = args.fold(String::from(""), |acc, arg| {acc + arg});
        
        if name == "" {
            let ns = resolve_object_err("", &mut self.outputs.err);
            println!("{}", unsafe{&*ns});
            return true;
        }

        let (basename, components) = split_qn_once(&name);
        
        let tname = CString::new(basename).unwrap();
        
        let entity = unsafe {rhdl_entity(ptr::null(), tname.as_ptr())};
        if !(entity.is_null()) {
            let iface = unsafe{*entity}.iface;
            return self.ls_internal(basename, iface, components);
        }
        
        let object = unsafe {rhdlo_get(ptr::null(), tname.as_ptr())};
        if !(object.is_null()) {
            return self.ls_internal(basename, object, components);
        }

        let connector = match self.rhdl.get_commands().get_connector(basename) {
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
            
        return self.ls_internal(basename, connector, components);
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

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let _ = (line, pos, ctx);
        Ok((0, Vec::with_capacity(0)))
    }
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
    fn complete(&self, _text: &str) -> Vec<String>
    {
        return Vec::<String>::new()
    }
}

struct ObjectCompleter {}

impl ObjectCompleter {
    fn complete_with_base<S: Selectable>(&self, base: *const S, base_qn: &str, component_qn: &str) -> Vec<String>
    {
        assert!(!base.is_null());

        let split = component_qn.rsplit_once('.');

        let (most, last) = match split {
            None => ("", component_qn),
            Some((m, l)) => (m,l)
        };

        assert!(base_qn != "" || most == "");

        let members = unsafe{(*base).members()};
        let mut result = Vec::<String>::new();

        for name in members {
            let last_trimmed = last.trim_start();
            if name.starts_with(&last_trimmed) {
                let name_lws =
                    String::from_utf8(vec![b' '; last.len() - last_trimmed.len()]).unwrap() + name;
                let cand = match base_qn {
                    "" => name_lws.to_string(),
                    bqn => match most {
                        "" => format!("{}.{}", bqn, name_lws),
                        mqn => format!("{}.{}.{}", bqn, mqn, name_lws)
                    }
                };
                result.push(cand);
            }
        }

        return result;
    }
}

impl CommandCompleter for ObjectCompleter {
    fn complete(&self, text: &str) -> Vec<String>
    {
        let split = text.rsplit_once('.');

        let (most, last) = match split {
            None => ("", text),
            Some((m, l)) => (m,l)
        };

        let base = resolve_object_noerr(most);

        if base.is_null() {
            return Vec::<String>::new()
        }

        self.complete_with_base(base, most, last)
    }
}

#[cfg(test)]
mod tests {
    use super::InnerRHDL;

    #[test]
    fn parse_empty() {
        let result = InnerRHDL::parse("");
        assert!(result.parsed == 0);
        assert!(result.id1.is_none());
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_name() {
        let result = InnerRHDL::parse("lol");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap() == "lol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_name_spaced() {
        let result = InnerRHDL::parse("       lol  ");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap() == "lol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_qn() {
        let result = InnerRHDL::parse("lol.bol");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap() == "lol.bol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_qn_spaced() {
        let result = InnerRHDL::parse("  lol .  bol ");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap() == "lol .  bol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_with_halfop() {
        let result = InnerRHDL::parse("lol.bol -");
        assert!(result.parsed == 2);
        assert!(result.id1.unwrap() == "lol.bol");
        assert!(result.operator.unwrap() == "-");
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_with_fullop() {
        let result = InnerRHDL::parse("lol.bol <-");
        assert!(result.parsed == 2);
        assert!(result.id1.unwrap() == "lol.bol");
        assert!(result.operator.unwrap() == "<-");
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_complete() {
        let result = InnerRHDL::parse("  alf.balf.lalf ->    ralf .schnalf   ");
        assert!(result.parsed == 3);
        assert!(result.id1.unwrap() == "alf.balf.lalf");
        assert!(result.operator.unwrap() == "->");
        assert!(result.id2.unwrap() == "ralf .schnalf");
    }
}