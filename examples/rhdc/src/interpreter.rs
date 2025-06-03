use std::collections::HashMap;
use std::io::Write;
use std::usize;

use rustyline::Context;
use rustyline::completion::{Completer,Pair};
use rustyline::error::ReadlineError;

use lazy_static::lazy_static;
use regex::Regex;
use const_format::formatcp;

pub const ALPHA: &'static str = "[A-Za-z]";
const CMDLINE: &'static str = formatcp!(r"^\s*({}+)\s*", ALPHA);

lazy_static! {
    static ref REGEX_CMDLINE: Regex = Regex::new(CMDLINE).unwrap();
}

pub trait Parameter : Sized {
    type Arg<'a> : Argument<'a>;

    fn regex() -> &'static Regex;
    fn usage() -> &'static str;

    fn completer() -> &'static dyn CommandCompleter;

    fn complete(arg: &str) -> Vec<String>
    {
        Self::completer().complete(arg)
    }

    fn position(arg: &str) -> (usize, usize) {
        match Self::regex().captures(arg) {
            Some(c) => {
                let arg = c.get(0).unwrap();
                (arg.start(), arg.end())
            },
            None => (0, 0)
        }
    }
}

pub enum ExtractErr<'a> {
    Match,
    Parse(&'a str)
}

pub trait Argument<'a> : Sized + Parameter
{
    fn parse<'b>(arg: &'a str) -> Result<Self, &'b str>;

    fn extract<'b>(args: &'a str) -> Result<(Self, usize), ExtractErr<'b>>
    {
        match Self::regex().captures(args) {
            Some(c) => {
                let m = c.get(0).unwrap();
                match Self::parse(m.as_str()) {
                    Ok(arg) => Ok((arg, m.end())),
                    Err(e) => Err(ExtractErr::Parse(e))
                }
            },
            None => Err(ExtractErr::Match)
        }
    }
}

pub trait CommandCompleter {
    fn complete(&self, text: &str) -> Vec<String>;
}

pub trait AbstractCommand<T> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), ExtractErr<'a>>;
    fn complete(&self, text: &str) -> (usize, Vec<String>);
    fn name(&self) -> &'static str;
    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error>;

    fn usage(&self, err: &mut dyn Write) {
        write!(err, "usage: {} ", self.name()).unwrap();
        self.param_usage(err).unwrap();
        writeln!(err).unwrap();
    }
}

type NullaryCommandFn<T> = fn(&mut T);
type UnaryCommandFn<T, Param> = for <'a> fn(&mut T, &<Param as Parameter>::Arg<'a>);
type OptUnaryCommandFn<T, Param> = for <'a> fn(&mut T, Option<&<Param as Parameter>::Arg<'a>>);
type BinaryCommandFn<T, Param0, Param1> = for <'a> fn(&mut T, &<Param0 as Parameter>::Arg<'a>, &<Param1 as Parameter>::Arg<'a>);

/* We need these structs to encapsulate the command function, because we
* cannot simply implement AbstractCommand for *CommandFn, because then
* GArg would be unconstrained */

struct NullaryCommand<T> {
    name: &'static str,
    func: NullaryCommandFn<T>,
}

impl <T> NullaryCommand<T> {
    fn new(name: &'static str, func: NullaryCommandFn<T>) -> Self
    {
        Self{name, func}
    }
}

struct UnaryCommand<T, P: Parameter> {
    name: &'static str,
    func: UnaryCommandFn<T, P>,
}

impl <T, P: Parameter> UnaryCommand<T, P> {
    fn new(name: &'static str, func: UnaryCommandFn<T, P>) -> Self
    {
        Self{name, func}
    }
}

struct OptUnaryCommand<T, P: Parameter> {
    name: &'static str,
    func: OptUnaryCommandFn<T, P>,
}

impl <T, P: Parameter> OptUnaryCommand<T, P> {
    fn new(name: &'static str, func: OptUnaryCommandFn<T, P>) -> Self
    {
        Self{name, func}
    }

}
struct BinaryCommand<T, P0: Parameter, P1: Parameter> {
    name: &'static str,
    func: BinaryCommandFn<T, P0, P1>,
}

impl <T, P0: Parameter, P1: Parameter> BinaryCommand<T, P0, P1> {
    fn new(name: &'static str, func: BinaryCommandFn<T, P0, P1>) -> Self
    {
        Self{name, func}
    }
}

impl<T> AbstractCommand<T> for NullaryCommand<T> {
    fn exec<'a>(&self, processor: &mut T, _args: &str) -> Result<(), ExtractErr<'a>> {
        (self.func)(processor);
        Ok(())
    }

    fn complete(&self, _args: &str) -> (usize, Vec<String>) {
        (0, Vec::new())
    }

    fn name(&self) -> &'static str {
        self.name
    }

    fn param_usage(&self, _err: &mut dyn Write) -> Result<(), std::io::Error>
    {
        Ok(())
    }
}

/* Here, GArg is constrained, because it is tied to self, which would not be
 * the case if Self == UnaryCommandFn */
impl<T, P: Parameter> AbstractCommand<T> for UnaryCommand<T, P> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), ExtractErr<'a>> {
        let arg = P::Arg::extract(args);

        match arg {
            Ok((arg, _)) => Ok((self.func)(processor, &arg)),
            Err(e) => Err(e)
        }
    }

    fn complete(&self, args: &str) -> (usize, Vec<String>) {
        (0, P::complete(args))
    }

    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error> {
        write!(err, "<{}>", P::usage())
    }

    fn name(&self) -> &'static str {
        return self.name;
    }
}

impl<T, P: Parameter> AbstractCommand<T> for OptUnaryCommand<T, P> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), ExtractErr<'a>> {
        match P::Arg::extract(args) {
            Ok((arg, _)) => Ok((self.func)(processor, Some(&arg))),
            Err(_) => Ok((self.func)(processor, None))
        }
    }

    fn complete(&self, args: &str) -> (usize, Vec<String>) {
        (0, P::complete(args))
    }

    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error> {
        write!(err, "[{}]", P::usage())
    }

    fn name(&self) -> &'static str {
        return self.name;
    }
}

impl<T, P0: Parameter, P1 : Parameter> AbstractCommand<T> for BinaryCommand<T, P0, P1> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), ExtractErr<'a>> {
        let (arg0, arg0end) = match P0::Arg::extract(args) {
            Ok((arg, pos)) => (arg, pos),
            Err(e) => return Err(e)
        };
        match P1::Arg::extract(&args[arg0end..]) {
            Ok((arg1, _)) => Ok((self.func)(processor, &arg0, &arg1)),
            Err(e) => Err(e)
        }
    }

    fn complete(&self, args: &str) -> (usize, Vec<String>) {
        let (arg0start, arg0end) = match P0::position(args) {
            (0, 0) => return (0, P0::complete(args)),
            pos => pos
        };

        let args_after_0 = &args[arg0end..];

        if args_after_0.len() == 0 {
            (arg0start, P0::complete(&args[arg0start..]))
        } else {
            (arg0end, P1::complete(args_after_0))
        }
    }

    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error> {
        write!(err, "<{}> <{}>", P0::usage(), P1::usage())
    }

    fn name(&self) -> &'static str {
        return self.name;
    }
}

pub fn command0<T: 'static> (
    name: &'static str, func: NullaryCommandFn<T>, cmds: &mut Commands<T>)
{
    cmds.insert(name, Box::new(NullaryCommand::new(name, func)));
}

pub fn command1<T: 'static, P: Parameter + 'static> (
    name: &'static str, func: UnaryCommandFn<T, P>, cmds: &mut Commands<T>)
{
    cmds.insert(name, Box::new(UnaryCommand::new(name, func)));
}

pub fn command1opt<T: 'static, P: Parameter + 'static> (
    name: &'static str, func: OptUnaryCommandFn<T, P>, cmds: &mut Commands<T>)
{
    cmds.insert(name, Box::new(OptUnaryCommand::new(name, func)));
}


pub fn command2<T: 'static, P0 : Parameter + 'static, P1 : Parameter + 'static> (
    name: &'static str, func: BinaryCommandFn<T, P0, P1>, cmds: &mut Commands<T>)
{
    cmds.insert(name, Box::new(BinaryCommand::new(name, func)));
}

pub trait Interpreter : Completer {
    fn eat(self : &mut Self, line : &String) -> bool;
    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool;
}

//pub type TheCommands<T, const N: usize> = [Box<dyn AbstractCommand<'static, T>>; N];
pub type Commands<T> = HashMap<&'static str, Box<dyn AbstractCommand<T> + 'static>>;

pub trait Processor : Sized {
    type Fallback : Interpreter<Candidate = Pair>;

    fn commands() -> Commands<Self>;

    fn fallback_mut(&mut self) -> &mut Self::Fallback;
    fn fallback(&self) -> &Self::Fallback;
    fn stderr(&mut self) -> &mut dyn Write;

    fn exec(&mut self, cmds: &Commands<Self>, cmd: &str, args: &str, orig: &String) -> bool {
        let command = match cmds.get(cmd) {
            Some(c) => c.as_ref(),
            None => return self.exec_fb(cmd, args, orig)
        };

        match command.exec(self, args) {
            Ok(_) => (),
            Err(e) => match e {
                ExtractErr::Match => command.usage(self.stderr()),
                ExtractErr::Parse(reason) => {
                    writeln!(self.stderr(), "{} failed: {}", cmd, reason).unwrap();
                }
            }
        }

        true
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
                let (argpos, mut argcand) = cmd.complete(args_trimmed);

                /* FIXME: Condition should be a member of (Abstract)Command */
                if command == "ls" {
                    assert!(argpos == 0);
                    argcand.append(&mut self.complete_object_contextually(args_trimmed));
                }

                let result: Vec<Pair> = argcand.into_iter().
                        map(|arg| {
                            let (_, new) = arg.split_at(args_trimmed.len() - argpos);
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
