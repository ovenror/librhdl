use std::collections::HashMap;
use std::usize;

use rustyline::Context;
use rustyline::completion::{Completer,Pair};
use rustyline::error::ReadlineError;

use lazy_static::lazy_static;
use regex::Regex;
use const_format::formatcp;

use crate::rhdc::{OBJECT_COMPLETER};

const ALPHA: &'static str = "[A-Za-z]";
pub const IDENTIFIER: &'static str = formatcp!(r"{0}[{0}0-9_]*", ALPHA);
const IDENTIFIERW: &'static str = formatcp!(r"\s*{}\s*", IDENTIFIER);
pub const QUALIFIED: &'static str = formatcp!(r"{0}(\.({0})?)*", IDENTIFIERW);
const CMDLINE: &'static str = formatcp!(r"^\s*({}+)\s+", ALPHA);

lazy_static! {
    static ref REGEX_CMDLINE: Regex = Regex::new(CMDLINE).unwrap();
    static ref REGEX_QN: Regex = Regex::new(formatcp!(r"^{}", QUALIFIED)).unwrap();
}

pub trait Parameter : Sized {
    type Arg<'a> : Argument<'a>;

    fn regex() -> &'static Regex;

    fn completer() -> &'static dyn CommandCompleter;

    fn complete(args: &str) -> Vec<String>
    {
        Self::completer().complete(args)
    }

    fn ends_at(args: &str) -> usize {
        match Self::regex().captures(args) {
            Some(c) => c.get(0).unwrap().end(),
            None => 0
        }
    }
}

pub trait Argument<'a> : Sized + Parameter
{
    fn parse(arg: &'a str) -> Self;

    fn extract(args: &'a str) -> Option<(Self, usize)>
    {
        match Self::regex().captures(args) {
            Some(c) => {
                let m = c.get(0).unwrap();
                Some((Self::parse(m.as_str()), m.end()))
            },
            None => None
        }
    }
}

impl Parameter for Vec<&str> {
    type Arg<'a> = QualifiedName<'a>;

    fn regex() -> &'static Regex {
        return &REGEX_QN
    }

    fn completer() -> &'static dyn CommandCompleter {
        &OBJECT_COMPLETER
    }
}

impl<'a> Argument<'a> for QualifiedName<'a>
{
    fn parse(arg: &'a str) -> Self {
        create_qn(arg)
    }
}

pub trait CommandCompleter {
    fn complete(&self, text: &str) -> Vec<String>;
}

pub type QualifiedName<'a> = Vec<&'a str>;

pub trait AbstractCommand<T> {
    fn exec<'a>(&'a self, processor: &'a mut T, args: &'a str) -> bool;
    fn complete(&self, text: &str) -> Vec<String>;
}

type NullaryCommandFn<T> = fn(&mut T) -> bool;
type UnaryCommandFn<T, Param> = for <'a> fn(&mut T, &<Param as Parameter>::Arg<'a>) -> bool;
type OptUnaryCommandFn<T, Param> = for <'a> fn(&mut T, Option<&<Param as Parameter>::Arg<'a>>) -> bool;
type BinaryCommandFn<T, Param0, Param1> = for <'a> fn(&mut T, &<Param0 as Parameter>::Arg<'a>, &<Param1 as Parameter>::Arg<'a>) -> bool;

/* We need these structs to encapsulate the command function, because we
* cannot simply implement AbstractCommand for *CommandFn, because then
* GArg would be unconstrained */
struct UnaryCommand<T, P: Parameter> {
    func: UnaryCommandFn<T, P>,
}

struct OptUnaryCommand<T, P: Parameter> {
    func: OptUnaryCommandFn<T, P>,
}
struct BinaryCommand<T, P0: Parameter, P1: Parameter> {
    func: BinaryCommandFn<T, P0, P1>,
}

impl<T> AbstractCommand<T> for NullaryCommandFn<T> {
    fn exec(&self, processor: &mut T, _args: &str) -> bool {
        self(processor)
    }

    fn complete(&self, _args: &str) -> Vec<String> {
        Vec::new()
    }
}

/* Here, GArg is constrained, because it is tied to self, which would not be
 * the case if Self == UnaryCommandFn */
impl<T, P: Parameter> AbstractCommand<T> for UnaryCommand<T, P> {
    fn exec<'a>(&'a self, processor: &'a mut T, args: &'a str) -> bool {
        let arg = P::Arg::<'a>::extract(args);

        match arg {
            Some((arg, _)) => (self.func)(processor, &arg),
            None => false
        }
    }

    fn complete(&self, args: &str) -> Vec<String> {
        P::complete(args)
    }
}

impl<T, P: Parameter> AbstractCommand<T> for OptUnaryCommand<T, P> {
    fn exec<'a>(&'a self, processor: &'a mut T, args: &'a str) -> bool {
        match P::Arg::<'a>::extract(args) {
            Some((arg, _)) => (self.func)(processor, Some(&arg)),
            None => (self.func)(processor, None)
        }
    }

    fn complete(&self, args: &str) -> Vec<String> {
        P::complete(args)
    }
}

impl<T, P0: Parameter, P1 : Parameter> AbstractCommand<T> for BinaryCommand<T, P0, P1> {
    fn exec(&self, processor: &mut T, args: &str) -> bool {
        let (arg0, arg0end) = match P0::Arg::extract(args) {
            Some((arg, pos)) => (arg, pos),
            None => return false
        };
        match P1::Arg::extract(&args[arg0end..]) {
            Some((arg1, _)) => (self.func)(processor, &arg0, &arg1),
            None => false
        }
    }

    fn complete(&self, args: &str) -> Vec<String> {
        let arg0end = match P0::ends_at(args) {
            0 => return P0::complete(args),
            pos => pos,
        };

        if arg0end == args.len() {      
            P0::complete(args)
        } else {
            P1::complete(args)
        }
    }
}

pub fn command0<'a, T: 'a> (
    func: NullaryCommandFn<T>) -> Box<dyn AbstractCommand<T> +'a>
{
    Box::new(func)
}

pub fn command1<'a, T: 'a, P: Parameter + 'a> (
    func: UnaryCommandFn<T, P>) -> Box<dyn AbstractCommand<T> + 'a>
{
    Box::new(UnaryCommand{func})
}

pub fn command1opt<'a, T: 'a, P: Parameter + 'a> (
    func: OptUnaryCommandFn<T, P>) -> Box<dyn AbstractCommand<T> + 'a>
{
    Box::new(OptUnaryCommand{func})
}

pub fn command2<'a, T: 'a, P0 : Parameter + 'a, P1 : Parameter + 'a> (
    func: BinaryCommandFn<T, P0, P1>) -> Box<dyn AbstractCommand<T> + 'a>
{
    Box::new(BinaryCommand{func})
}

pub trait Interpreter : Completer {
    fn eat(self : &mut Self, line : &String) -> bool;
    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool;
}

//pub type TheCommands<T, const N: usize> = [Box<dyn AbstractCommand<'static, T>>; N];
pub type Commands<T> = HashMap<&'static str, Box<dyn AbstractCommand<T>>>;

pub trait Processor : Sized {
    type Fallback : Interpreter<Candidate = Pair>;

    fn commands() -> Commands<Self>;

    fn fallback_mut(&mut self) -> &mut Self::Fallback;
    fn fallback(&self) -> &Self::Fallback;

    fn exec(&mut self, cmds: &Commands<Self>, cmd: &str, args: &str, orig: &String) -> bool {
        let command = match cmds.get(cmd) {
            Some(c) => c.as_ref(),
            None => return self.exec_fb(cmd, args, orig)
        };

        command.exec(self, args)
    }

    fn eat_fb(self : &mut Self, line: &String) -> bool
    {
        self.fallback_mut().eat(line)
    }

    fn exec_fb(&mut self, cmd: &str, args: &str, orig: &String) -> bool {
        self.fallback_mut().exec(cmd, args, orig)
    }

    fn complete_object_contextually(&self, line: &str) -> Vec<String>;

    fn complete(&self, cmds: &Commands<Self>, line: &str, pos: usize, _ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        let line_trimmed = line.trim_start();
        let (command, args) = match line_trimmed.split_once(' ') {
            Some((c, a)) => (c,a),
            None => {
                if line.ends_with(' ') {
                    (line_trimmed, "")
                }
                else {
                    let cmditer2 = cmds.keys();
                    let cmds : Vec<Pair> = cmditer2.
                            filter(|cmd| cmd.starts_with(&line_trimmed)).
                            map(|cmd| {let (_, last) = cmd.split_at(pos + line_trimmed.len() - line.len()); last}).
                            //map(|cmd| cmd.to_string()).
                            map(|cmd| Pair{
                                display: cmd.to_string(),
                                replacement: cmd.to_string()})
                            .collect();
 
                    return Ok((pos, cmds.to_vec()))
                }
            }
        };

        let optcmd = cmds.get(command); 
        
        match optcmd {
            Some(cmd) => {
                let args_trimmed = args.trim_start();
                let mut argcand = cmd.complete(args_trimmed);

                /* FIXME: Condition should be a member of (Abstract)Command */
                if command == "ls" {
                    argcand.append(&mut self.complete_object_contextually(args_trimmed));
                }

                let result: Vec<Pair> = argcand.into_iter().
                        map(|arg| {
                            let (_, new) = arg.split_at(args_trimmed.len());
                            new.to_string()}).
                        map(|rep| Pair {
                                display: rep.to_string(),
                                replacement: rep.to_string()}).
                        collect();
                return Ok((pos, result))
            }
            _ => ()
        }

        return Ok((pos, Vec::new()))
    }
    
    fn complete_fb(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        self.fallback().complete(line, pos, ctx)
    }
}

pub struct SimpleInterpreter<C : Processor> {
    processor : C,
    commands: Commands<C>
}

impl<C: Processor> SimpleInterpreter<C> {
    pub fn new(processor : C) -> Self {
        Self {processor: processor, commands: C::commands()}
    }

    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool {
        self.processor.exec(&self.commands, cmd, args, orig)
    }

    pub fn get_processor(&self) -> &C {
        &self.processor
    }

    fn crop_candidate(c: &Pair, by: usize) -> Pair {
        let (_, d) = c.display.split_at(by);
        let (_, r) = c.replacement.split_at(by);

        Pair{display: d.to_string(), replacement: r.to_string()}
    }

    fn crop_candidates(v: &Vec<Pair>, by: usize) -> Vec<Pair> {
        v.into_iter().map(|c| Self::crop_candidate(c, by)).collect()
    }
}

pub fn create_qn<'a>(qnstr: &'a str) -> QualifiedName<'a>
{
    qnstr.split(".").into_iter().map(|component| component.trim()).collect()
}

impl<C : Processor> Interpreter for SimpleInterpreter<C>
{
    fn eat(self : &mut Self, line : &String) -> bool {
        let caps = match REGEX_CMDLINE.captures(line) {
            Some(c) => c,
            None => return self.processor.eat_fb(line)
        };

        if line.trim().is_empty() {
            return true;
        }

        let commandcap = caps.get(1);
        assert!(caps.get(1).is_some());

        let (commandstr, argsstr) = line.split_at(commandcap.unwrap().end());

        self.exec(commandstr, argsstr, line)
    }

    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool {
        self.processor.exec(&self.commands, cmd, args, orig)
    }
}

impl<'a, C : Processor> Completer for SimpleInterpreter<C> {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let (mut p1, mut vec1) = match self.processor.complete(&self.commands, line, pos, ctx) {
            Ok(result) => result,
            _ => (0, Vec::<Pair>::new())
        };
        
        let (p2, mut vec2) = match self.processor.complete_fb(line, pos, ctx) {
            Ok(result) => result,
            _ => (0, Vec::<Pair>::new())
        };

        if vec1.is_empty() {
            return Ok((p2, vec2))
        }

        if vec2.is_empty() {
            return Ok((p1, vec1))
        }

        if p1 < p2 {
            vec1 = Self::crop_candidates(&vec1, p2 - p1);
            p1 = p2;
        }

        if p2 < p1 {
            vec2 = Self::crop_candidates(&vec2, p1 - p2);
        }

        vec1.append(&mut vec2);
        return Ok((p1, vec1))
    }
}
