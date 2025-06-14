extern crate regex;
extern crate lazy_static;
extern crate const_format;

use crate::interpreter;
use crate::interpreter::CommandCompleter;
use crate::interpreter::Interpreter;
use crate::interpreter::Parameter;
use crate::interpreter::Argument;
use crate::interpreter::Commands;
use crate::interpreter::{command0, command1, command2, command3};
use crate::console::Outputs;
use crate::console::SimpleConsoleInterpreter;
use crate::console::Processor;
use crate::util::split_qn_once;
use crate::librhdl::*;
use crate::resolve::*;
use crate::librhdl_access::*;

use lazy_static::lazy_static;
use std::iter;
use std::io::Write;
use std::process::exit;
use std::ffi::CString;
use std::ptr;
use std::collections::hash_map::HashMap;
use regex::Regex;
use regex::Match;
use const_format::formatcp;

use rustyline::error::ReadlineError;
use rustyline::Context;
use rustyline::completion::{Completer, Pair};

const IDENTIFIER: &'static str = formatcp!(r"{0}[{0}0-9_]*", interpreter::ALPHA);
const IDENTIFIER_WWS: &'static str = formatcp!(r"\s*{}\s*", IDENTIFIER);
const QUALIFIED: &'static str = formatcp!(r"{0}(\.{0})*", IDENTIFIER_WWS);
const QUALIFIED_PARTIAL: &'static str = formatcp!(r"({0}(\.{0})*\.?)?\s*", IDENTIFIER_WWS);
const OPERATOR: &'static str = r":|->|<-";
const OPERATOR_PARTIAL: &'static str = r"-|<";
const RHDD: &'static str = formatcp!(r"^(({0})(({1})(({0})|({2}))?|({3}))?|({2}))$", QUALIFIED, OPERATOR, QUALIFIED_PARTIAL, OPERATOR_PARTIAL);

const PARTIAL_ID1_POS: usize = 13;
const ID1_POS: usize = 2;
const PARTIAL_OP_POS: usize = 12;
const OP_POS: usize = 5;
const ID2_POS: usize = 7;
const PARTIAL_ID2_POS: usize = 9;

lazy_static! {
    static ref REGEX_ID_ONLY: Regex = Regex::new(formatcp!(r"^{}$", IDENTIFIER)).unwrap();
    static ref REGEX_ID: Regex = Regex::new(formatcp!(r"^{}", IDENTIFIER)).unwrap();
    static ref REGEX_ID_COMPLETABLE: Regex = Regex::new(formatcp!(r"^{}?$", IDENTIFIER)).unwrap();
    static ref REGEX_QN: Regex = Regex::new(formatcp!(r"^{}", QUALIFIED)).unwrap();
    static ref REGEX_QN_OR_WS_ONLY: Regex = Regex::new(formatcp!(r"^({}|\s*)$", QUALIFIED)).unwrap();
    static ref REGEX_RHDD: Regex = Regex::new(RHDD).unwrap();
    static ref REGEX_NONE: Regex = Regex::new(r"^$").unwrap();
}

pub struct QualifiedName<'a> {
    components: Vec<&'a str>
}

impl<'a> QualifiedName<'a> {
    pub fn new(components: Vec<&'a str>) -> Self {
        assert!(components.iter().all(|c| c != &""));
        Self{components}
    }

    pub fn from(qnstr: &'a str) -> Self
    {
        assert!(REGEX_QN_OR_WS_ONLY.captures(qnstr).is_some());

        if qnstr.trim() == "" {
            return Self::new(Vec::new());
        }

        let result: Vec<&'a str> = qnstr.split(".").into_iter().map(|component| component.trim()).collect();

        Self::new(result)
    }

    pub fn try_from(qnstr: &'a str) -> Result<Self, ()>
    {
        match REGEX_QN_OR_WS_ONLY.captures(qnstr) {
            Some(c) => Ok(Self::from(c.get(0).unwrap().as_str())),
            None => Err(())
        }
    }

    pub fn len(&self) -> usize
    {
        self.components.len()
    }

    pub fn slice(&'a self) -> &'a [&'a str]
    {
        &self.components
    }

    pub fn split_last(&'a self) -> Option<(&'a &'a str, &'a [&'a str])>
    {
        self.components.split_last()
    }

    pub fn split_first(&'a self) -> Option<(&'a &'a str, &'a [&'a str])>
    {
        self.components.split_first()
    }

}

impl Parameter for &str {
    type Arg<'a> = &'a str;

    fn regex() -> &'static Regex {
        return &REGEX_ID
    }

    fn usage() -> &'static str {
        "identifier"
    }

    fn completer() -> &'static dyn CommandCompleter {
        &NO_COMPLETER
    }
}

impl<'a> Argument<'a> for &'a str
{
    fn parse_extracted<'b>(arg: &'a str) -> Result<Self, &'b str> {
        Ok(arg)
    }
}

impl<'a> Parameter for QualifiedName<'a> {
    type Arg<'b> = QualifiedName<'b>;

    fn regex() -> &'static Regex {
        return &REGEX_QN
    }

    fn usage() -> &'static str {
        "qualified name"
    }

    fn completer() -> &'static dyn CommandCompleter {
        &OBJECT_COMPLETER
    }
}

impl<'a> Argument<'a> for QualifiedName<'a>
{
    fn parse_extracted<'b>(arg: &'a str) -> Result<Self, &'b str> {
        Ok(QualifiedName::from(arg))
    }
}

impl Parameter for &rhdl_object_t {
    type Arg<'a> = &'a rhdl_object_t;

    fn regex() -> &'static Regex {
        QualifiedName::regex()
    }

    fn usage() -> &'static str {
        QualifiedName::usage()
    }

    fn completer() -> &'static dyn CommandCompleter {
        QualifiedName::completer()
    }
}

impl<'a> Argument<'a> for &'a rhdl_object_t {
    fn parse_extracted<'b>(arg: &'a str) -> Result<Self, &'b str> {
        let qn = QualifiedName::from(arg);
        let root_resolved = resolve_object_noerr(qn.slice());

        if root_resolved.is_null() {
                let err = errstr();
                let entities = resolve_object_noerr(&["entities"]);
                assert!(!entities.is_null());
                let entities_resolved = resolve_with_base_noerr(entities, qn.slice());

                if entities_resolved.is_null() {
                    Err(err)
                } else {
                    Ok(unsafe{&*entities_resolved})
                }
        } else {
            Ok(unsafe{&*root_resolved})
        }
    }
}

struct InnerRHDLParseResult<'a> {
    parsed: u8,
    id1: Option<Match<'a>>,
    operator: Option<Match<'a>>,
    id2: Option<Match<'a>>
}

impl<'a> InnerRHDLParseResult<'a> {
    fn new(
        parsed: u8, id1: Option<Match<'a>>, operator: Option<Match<'a>>,
        id2: Option<Match<'a>>) -> InnerRHDLParseResult<'a>
    {
        InnerRHDLParseResult {
            parsed: parsed,
            id1: id1,
            operator: operator,
            id2: id2
        }
    }
}

pub struct InnerRHDL {
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
        //println!();
        //println!("MATCH: >>>{}", cmd);

        let cap = match REGEX_RHDD.captures(cmd) {
            Some(v) => v,
            None => return InnerRHDLParseResult::new(0, None, None, None)
        };

        //dbg!(&cap);

        let id1 = Some(match cap.get(ID1_POS) {
            None => match cap.get(PARTIAL_ID1_POS) {
                None => return InnerRHDLParseResult::new(0, None, None, None),
                Some(c) => c
            },
            Some(c) => c
        });

        let operator = Some(match cap.get(OP_POS) {
            None => match cap.get(PARTIAL_OP_POS) {
                None => return InnerRHDLParseResult::new(1, id1, None, None),
                Some(c) => c
            },
            Some(c) => c
        });

        match cap.get(ID2_POS) {
            None => match cap.get(PARTIAL_ID2_POS) {
                None => InnerRHDLParseResult::new(2, id1, operator, None),
                id2 => InnerRHDLParseResult::new(3, id1, operator, id2)
            },
            id2 => InnerRHDLParseResult::new(4, id1, operator, id2)
        }
    }

    fn define(&mut self, qn: &QualifiedName, stateless: bool) -> bool {
        if self.active {
            panic!("already defining {}", self.ename);
        }

        assert!(qn.len() >= 1);

        let (&ename, ns_qn) = qn.split_last().unwrap();

        let nspace = resolve_namespace_err(
            if ns_qn.is_empty() {&["entities"]} else {ns_qn},
            &mut self.outputs.err);

        if nspace.is_null() {
            return true;
        }

        let cename = CString::new(ename).unwrap();
   
        let mode = if stateless {Flags_F_CREATE_STATELESS} else {Flags_F_CREATE_STATEFUL};

        unsafe {    
            self.structure = rhdl_begin_structure(nspace, cename.as_ptr(), mode);
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

        match QualifiedName::try_from(components) {
            Ok(qn) => resolve_with_base_err(base, qn.slice(), basename, &mut self.outputs.err),
            Err(_) => ptr::null()
        }
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
        if name == self.ename {
            writeln!(self.outputs.err, "Cannot instantiate a component with the same name as the currently defined structure ({})", name).unwrap();
            return
        }

        if self.components.contains_key(name) {
            writeln!(self.outputs.err, "Component \"{}\" already exists", name).unwrap();
            return
        }

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

    fn symbols(&self)
            ->  std::iter::Chain<
                    std::iter::Once<&String>,
                    std::collections::hash_map::Keys<'_, String, *const rhdl_connector>>
    {
        return iter::once(&self.ename).chain(self.components.keys());
    }

    fn is_symbol(&self, name: &str) -> bool {
        name == self.ename || self.components.contains_key(name)
    }

    pub fn complete_own_object(&self, qn_str: &str) -> (usize, Vec<String>) {
        let (basename, basename_trimmed, components) = match qn_str.split_once('.') {
            None => (qn_str, qn_str.trim(), ""),
            Some((b, c)) => {
                let b_trimmed = b.trim();
                if b_trimmed == "" {
                    return (0, Vec::new())
                } else {
                    (b, b_trimmed, c)
                }
            }
        };

        if self.is_symbol(basename_trimmed) {
            let (basedmost, last) = match qn_str.rsplit_once(".") {
                None => return (0, vec![qn_str.to_string()]),
                Some((m,l)) => (m,l)
            };

            let ((), most) = match basedmost.split_once(".") {
                None => ((), ""),
                Some((_, m)) => ((), m)
            };

            let most_qn = match QualifiedName::try_from(most) {
                Ok(qn) => qn,
                Err(_) => return (0, Vec::new())
            };

            let most_resolved = resolve_with_base_noerr(
                self.get_connector(basename_trimmed), most_qn.slice());

            if most_resolved.is_null() {
                return (0, Vec::new());
            }

            return OBJECT_COMPLETER.complete_last_component(most_resolved, basedmost.len(), last)
        }

        if components != "" {
            return (0, Vec::new());
        }

        let mut cand = Vec::<String>::new();

        for symbol in self.symbols() {
            if symbol.starts_with(basename_trimmed) {
                cand.push(symbol.to_string());
            }
        }

        assert!(basename_trimmed.len() == basename.trim_start().len());

        return (basename.len() - basename_trimmed.len(), cand);
    }

    fn complete_2nd_id(&self, qn: &str, entity: bool) -> (usize, Vec<String>) {
        if entity {
            /*
            let (most, last) = match qn.rsplit_once(".") {
                Some((m,l)) => (m,l),
                None => ("", qn)
            };
            */

            let entities_name = CString::new("entities").unwrap();
            let entities = unsafe{rhdlo_get(ptr::null(), entities_name.as_ptr())};
            let (pos, cand_entity) = OBJECT_COMPLETER.complete_with_base(entities, qn);

            if cand_entity.is_empty() {
                OBJECT_COMPLETER.complete(qn)
            } else {
                (pos, cand_entity)
            }
        } else {
            self.complete_own_object(qn)
        }
    }

    fn complete_object_contextually(&self, line: &str) -> (usize, Vec<String>)
    {
        if self.is_active() {
            self.complete_own_object(line)
        } else {
            (0, Vec::new())
        }
    }
}

impl interpreter::Interpreter for InnerRHDL {
    fn eat(&mut self, line: &String) -> bool
    {
        if !self.is_active() {
            return true
        }

        let parsed = Self::parse(line);

        if parsed.parsed != 4 {
            return false;
        }

        let id1 = parsed.id1.unwrap().as_str().trim();
        let id2 = parsed.id2.unwrap().as_str().trim();

        match parsed.operator.unwrap().as_str() {
            "->" => self.connect(id1, id2),
            "<-" => self.connect(id2, id1),
            ":" => self.instantiate(id1, id2),
            _ => return false
        }

        return true;
    }

    fn exec(self : &mut Self, _command: &str, _args: &str, orig: &String) -> bool {
        self.eat(orig)
    }
}

impl Completer for InnerRHDL {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, _ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let parsed = Self::parse(&line[..pos]);

        if parsed.parsed == 0{
            return Ok((0, Vec::<Pair>::new()))
        }

        let result = match parsed.parsed {
            1 => {
                let id1 = parsed.id1.unwrap();
                assert!(id1.start() == 0);
                let id1str = id1.as_str();

                if  !id1str.is_empty() &&
                    id1str.chars().last().unwrap().is_whitespace() /* &&
                    match QualifiedName::try_from(id1str) {
                        Err(()) => false,
                        Ok(qn) => !self.get_connectible(id1str).is_null()
                    } */
                {
                    (pos, vec!["-> ", "<- "].into_iter().map(|s| s.to_string()).collect())
                } else {
                    let (ocandstart, ocand) = self.complete_own_object(id1str);

                    match ocand.len() {
                        0 => if REGEX_ID_ONLY.is_match(line) {
                                (pos, vec![" : ".to_string()])
                            } else {
                                (0, Vec::new())
                            },
                        _ => (ocandstart, ocand)
                    }
                }
            }
            2 => {
                let op = parsed.operator.unwrap().as_str();

                (pos - 1, match op {
                    "-" => vec!["-> ".to_string()],
                    "<" => vec!["<- ".to_string()],
                    _ => Vec::new()
                })
            }
            3 | 4 => {
                let id2 = parsed.id2.unwrap();
                let (cand_pos, cand) = self.complete_2nd_id(
                    id2.as_str(), parsed.operator.unwrap().as_str() == ":");
                (id2.start() + cand_pos, cand)
            }
            _ => panic!("wat: {}", parsed.parsed)
        };

        let (cand_pos, vec) = result;

        Ok((pos, vec.into_iter().map(|c| Pair{
            display: c.to_string(), replacement: c[pos - cand_pos..].to_string()}).collect()))
    }
}

pub struct OuterRHDL {
    outputs: Outputs,
    rhdd: InnerRHDL
}

impl OuterRHDL {
    fn new(outputs: Outputs) -> Self {
        let oc = outputs.clone();

        OuterRHDL {
            outputs: outputs,
            rhdd: InnerRHDL::new(oc)
        }
    }
    
    fn define(&mut self, arg: &QualifiedName) {
        self.define_internal(arg, true)
    }

    fn stateful(&mut self, arg: &QualifiedName) {
        self.define_internal(arg, false)
    }

    fn define_internal(&mut self, arg: &QualifiedName, stateless: bool) {
        if self.rhdd.is_active() {
            writeln!(self.outputs.err, "Already defining {}", self.rhdd.get_ename()).unwrap();
        }

        assert!(arg.len() > 0);

        if arg.len() == 0 {
            writeln!(self.outputs.err, "usage: define <qualified name>").unwrap();
        }

        self.rhdd.define(arg, stateless);
    }

    fn enddef(&mut self) {
        if self.rhdd.is_active() {
            self.rhdd.enddef();
        }
        else {
            writeln!(self.outputs.err, "not currently defining a structure").unwrap();
        }
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

impl<'a> interpreter::Processor for OuterRHDL {
    type Fallback = InnerRHDL;
            
    fn commands() -> Commands<Self> {
        let mut commands: Commands<Self> = HashMap::new();

        command1::<OuterRHDL, QualifiedName>("def", Self::define, &mut commands);
        command1::<OuterRHDL, QualifiedName>("define", Self::define, &mut commands);
        command1::<OuterRHDL, QualifiedName>("stateful", Self::stateful, &mut commands);
        command0("enddef", Self::enddef, &mut commands);

        commands
    }

    fn stderr(&mut self) -> &mut dyn Write {
        self.outputs.err.as_mut()
    }

    fn fallback(&self) -> &Self::Fallback
    {
        &self.rhdd
    }

    fn fallback_mut(&mut self) -> &mut Self::Fallback
    {
        &mut self.rhdd
    }

    fn exec_fb(self : &mut Self, _command: &str, _args: &str, orig: &String)
        -> bool
    {
        if !self.rhdd.is_active() {
            return false;
        }

        self.rhdd.eat(orig)
    }
    
    fn complete_object_contextually(&self, line: &str) -> (usize, Vec<String>)
    {
        return self.rhdd.complete_object_contextually(line);
    }

    fn complete_fb(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        if !self.rhdd.is_active() {
            return Ok((pos, Vec::<Pair>::new()))
        }

        return self.rhdd.complete(line, pos, ctx)
    }

    fn prompt_size(&self) -> usize {
        self.prompt_info().len() + 2
    }

    fn interactive(&self) -> bool {
        self.outputs.interactive
    }
}

impl<'a> Processor for OuterRHDL {
    fn prompt_info(&self) -> &str {
        match self.rhdd.active {
            true => &self.rhdd.get_ename(),
            false => ""
        }
    }
}
    
pub struct RHDC {
    outputs: Outputs,
    rhdl: SimpleConsoleInterpreter<OuterRHDL>
}

impl RHDC {
    pub fn new(outputs: Outputs) -> Self {
        let oc = outputs.clone();

        RHDC {
            outputs: outputs,
            rhdl: SimpleConsoleInterpreter::new(OuterRHDL::new(oc))
        }
    }

    fn quit(&mut self) {
        writeln!(self.outputs.out, "Quitting.").unwrap();
        exit(0);
    }

    fn panic(&mut self) {
        writeln!(self.outputs.out, "ok panic").unwrap();
        panic!();
    }

    fn ls_internal<I: Selectable>(&mut self, basename: &str, base: *const I, qn: &QNSlice) {
        let resolved = resolve_with_base_err(base, qn, basename, &mut self.outputs.err);

        if resolved.is_null() {
            return;
        }
        println!("{}", unsafe{&*resolved});
    }
   
    fn ls(&mut self, arg : &Option<QualifiedName>) {
        let qn = match arg {
            Some(q) => q,
            None => {
                let ns = resolve_object_err(&[], &mut self.outputs.err);
                println!("{}", unsafe{&*ns});
                return;
            }
        };

        assert!(qn.len() >= 1);

        let (basename, components) = match qn.split_first() {
            Some((b,c)) => (*b,c),
            None => panic!()
        };

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
                return;
            }
        };

        if connector.is_null() {
            writeln!(self.outputs.err, "{} is neither a known entity, nor an identifier used in the current structure definition", basename).unwrap();
            return;
        }
            
        return self.ls_internal(basename, connector, components);
    }

    fn synth(&mut self, name: &&str) {
        let ec;
        let tname = CString::new(*name).unwrap();
        unsafe {ec = rhdl_print_commands(tname.as_ptr());}
        if ec != 0 {
            write!(self.outputs.err, "{}", name).unwrap();
            perror(&mut self.outputs.err);
        }
    }

    fn transform(&mut self, representation: &&rhdl_object_t, transformation: &&rhdl_object_t, name: &&str)
    {
        let result_name = CString::new(*name).unwrap();
        let result = unsafe{rhdlo_transform(*representation, *transformation, result_name.as_ptr())};

        if result.is_null() {
            perror(&mut self.outputs.err);
        }
    }

    fn test(&mut self, _: &&rhdl_object_t, _: &&rhdl_object_t) {}
}

impl<'a> Completer for RHDC {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let _ = (line, pos, ctx);
        Ok((0, Vec::with_capacity(0)))
    }
}

impl interpreter::Processor for RHDC {
    type Fallback = SimpleConsoleInterpreter<OuterRHDL>;

    fn stderr(&mut self) -> &mut dyn Write {
        self.outputs.err.as_mut()
    }

    fn fallback_mut(&mut self) -> &mut Self::Fallback {
        &mut self.rhdl
    }

    fn fallback(&self) -> &Self::Fallback {
        &self.rhdl
    }

    fn complete_object_contextually(&self, line: &str) -> (usize, Vec<String>)
    {
        return self.rhdl.get_commands().complete_object_contextually(line);
    }
    
    fn commands() -> Commands<Self> {
        let mut commands: Commands<Self> = HashMap::new();

        command0("quit", Self::quit, &mut commands);
        command0("panic", Self::panic, &mut commands);
        command1::<RHDC, Option<QualifiedName>>("ls", Self::ls, &mut commands);
        command1::<RHDC, &str>("synth", Self::synth, &mut commands);
        command2::<RHDC, &rhdl_object_t, &rhdl_object_t>("test", Self::test, &mut commands);
        command3::<RHDC, &rhdl_object_t, &rhdl_object_t, &str>("transform", Self::transform, &mut commands);

        commands
    }

    fn prompt_size(&self) -> usize {
        self.prompt_info().len() + 2
    }

    fn interactive(&self) -> bool {
        self.outputs.interactive
    }
}

impl Processor for RHDC {
    fn prompt_info(&self) -> &str {
        self.rhdl.get_commands().prompt_info()
    }
}

pub static OBJECT_COMPLETER : ObjectCompleter =  ObjectCompleter{};
pub static NO_COMPLETER : NoCompleter =  NoCompleter{};

pub struct NoCompleter {}

impl CommandCompleter for NoCompleter {
    fn complete(&self, _text: &str) -> (usize, Vec<String>) {
        (0, Vec::new())
    }
}

pub struct ObjectCompleter {}

impl ObjectCompleter {
    fn complete_with_base<S: Selectable>(&self, base: *const S, qn_str: &str) -> (usize, Vec<String>)
    {
        assert!(!base.is_null());

        let (most_resolved, most_len, last) = match qn_str.rsplit_once(".") {
            Some((most, l)) => {
                let qn = match QualifiedName::try_from(most) {
                    Ok(result) => result,
                    Err(_) => return (0, Vec::new())
                };

                let mb = resolve_with_base_noerr(base, qn.slice());

                if mb.is_null() {
                    return (0, Vec::new());
                }

                (mb, most.len(), l)
            },
            None => (base, 0, qn_str)
        };

        self.complete_last_component(most_resolved, most_len, last)
    }


    fn complete_last_component<S: Selectable>(&self, base: *const S, base_qn_len: usize, last: &str) -> (usize, Vec<String>)
    {
        assert!(!base.is_null());
        assert!(!last.contains("."));

        let mut result = Vec::<String>::new();
        let last_trimmed = last.trim_start();

        assert!(!last_trimmed.contains(char::is_whitespace));

        let lws = last.len() - last_trimmed.len();
        let pos = if base_qn_len == 0 {0} else {base_qn_len + 1} + lws;
        let members = unsafe{(*base).members()};
        let mut current_name: &str = "";

        for name in members {
            if !name.starts_with(&last_trimmed) {
                continue;
            }

            result.push(name.to_string());
            current_name = name;
        }

        let mut current_base = base;

        if result.len() != 1 {
            return (pos, result);
        }

        loop {
            assert!(current_name != "");
            let oldresult = result.drain(..).next().unwrap();

            //println!("oldresult is: {}", &oldresult);
            //println!("name is: {}", current_name);

            current_base = unsafe{(*current_base).select(current_name)};
            assert!(!current_base.is_null());
            let mut current_members = unsafe{(*current_base).members()};

            loop {
                let next = current_members.next();

                if next.is_none() {
                    break;
                }

                current_name = next.unwrap();
                result.push(format!("{}.{}", &oldresult, current_name));
            }

            if result.len() != 1 {
                return (pos, vec![oldresult])
            }
        }
    }
}

impl<'a> CommandCompleter for ObjectCompleter {
    fn complete(&self, text: &str) -> (usize, Vec<String>)
    {
        let (most, last) = match text.rsplit_once('.') {
            None => ("", text),
            Some((m, l)) => {
                if m.trim() == "" {
                    return (0, Vec::new())
                } else {
                    (m,l)
                }
            }
        };

        let most_qn = match QualifiedName::try_from(most) {
            Ok(qn) => qn,
            Err(_) => return (0, Vec::new())
        };

        let base = resolve_object_noerr(most_qn.slice());

        if base.is_null() {
            return (0, Vec::new())
        }

        self.complete_last_component(base, most.len(), last)
    }
}

#[cfg(test)]
mod tests {
    use crate::resolve::resolve_object_noerr;

    use super::{InnerRHDL, OBJECT_COMPLETER};

    #[test]
    fn parse_empty() {
        let result = InnerRHDL::parse("");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_name() {
        let result = InnerRHDL::parse("lol");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "lol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_name_with_period() {
        let result = InnerRHDL::parse("lol.");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "lol.");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_name_spaced() {
        let result = InnerRHDL::parse("       lol  ");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "       lol  ");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_qn() {
        let result = InnerRHDL::parse("lol.bol");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "lol.bol");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_qn_spaced() {
        let result = InnerRHDL::parse("  lol .  bol ");
        assert!(result.parsed == 1);
        assert!(result.id1.unwrap().as_str() == "  lol .  bol ");
        assert!(result.operator.is_none());
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_with_halfop() {
        let result = InnerRHDL::parse("lol.bol -");
        assert!(result.parsed == 2);
        assert!(result.id1.unwrap().as_str() == "lol.bol ");
        assert!(result.operator.unwrap().as_str() == "-");
        assert!(result.id2.is_none());
    }

    #[test]
    fn parse_with_fullop() {
        let result = InnerRHDL::parse("lol.bol <-");
        assert!(result.parsed == 3);
        assert!(result.id1.unwrap().as_str() == "lol.bol ");
        assert!(result.operator.unwrap().as_str() == "<-");
        assert!(result.id2.unwrap().as_str() == "");
    }

    #[test]
    fn parse_complete() {
        let result = InnerRHDL::parse("  alf.balf.lalf ->    ralf .schnalf   ");
        assert!(result.parsed == 4);
        assert!(result.id1.unwrap().as_str() == "  alf.balf.lalf ");
        assert!(result.operator.unwrap().as_str() == "->");
        assert!(result.id2.unwrap().as_str() == "    ralf .schnalf   ");
    }

    use crate::rhdc::QualifiedName;

    #[test]
    fn object_completer_complete_last_single() {
        let base_qn = "entities.Inverter.interface";
        let (pos, cand) = OBJECT_COMPLETER.complete_last_component(
            resolve_object_noerr(QualifiedName::from(base_qn).slice()), base_qn.len(), "i");
        assert!(pos == base_qn.len() + 1);
        assert!(cand.len() == 1);
        dbg!(cand.last().unwrap());
        assert!(cand.last().unwrap() == "in.direction");
    }

    #[test]
    fn object_completer_complete_last_none() {
        let base_qn = "entities.Inverter.interface.in.direction";
        let (pos, cand) = OBJECT_COMPLETER.complete_last_component(
            resolve_object_noerr(QualifiedName::from(base_qn).slice()), base_qn.len(), "");
        assert!(pos == base_qn.len() + 1);
        assert!(cand.is_empty());
    }
}