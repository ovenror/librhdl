extern crate paste;

use std::collections::HashMap;
use std::io::Write;
use std::usize;

use rustyline::Context;
use rustyline::completion::{Completer,Pair};
use rustyline::error::ReadlineError;

use lazy_static::lazy_static;
use regex::Regex;
use const_format::formatcp;

use paste::paste;

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

    fn usage_gr() -> String {
        return "<".to_string() + Self::usage() + ">"
    }

    fn complete(args: &str, from: usize) -> Vec<String>
    {
        Self::completer().complete(&args[from..])
    }

    fn position(args: &str, from: usize) -> (usize, usize) {
        match Self::regex().captures(&args[from..]) {
            Some(c) => {
                let arg = c.get(0).unwrap();
                (from + arg.start(), from + arg.end())
            },
            None => (0, 0)
        }
    }
}

pub enum ExtractErr<'a> {
    Missing,
    Match,
    Parse(&'a str)
}

pub trait Argument<'a> : Sized + Parameter
{
    fn parse<'b>(arg: &'a str) -> Result<Self, &'b str>;

    fn extract<'b>(args: &'a str, from: usize) -> Result<(Self, usize), ExtractErr<'b>> {
        if from >= args.len() {
            Err(ExtractErr::Missing)
        } else {
            Self::extract_something(args, from)
        }
    }

    fn extract_something<'b>(args: &'a str, from: usize) -> Result<(Self, usize), ExtractErr<'b>>
    {
        assert!(from < args.len());

        match Self::regex().captures(&args[from..]) {
            Some(c) => {
                let m = c.get(0).unwrap();
                match Self::parse(m.as_str()) {
                    Ok(arg) => {
                        let end = from + m.end();
                        let mut delim = args[end - 1..].chars();
                        if delim.next().unwrap().is_whitespace() || delim.next().unwrap_or(' ').is_whitespace() {
                            Ok((arg, end))
                        } else {
                            Err(ExtractErr::Match)
                        }
                    },
                    Err(e) => Err(ExtractErr::Parse(e))
                }
            },
            None => Err(ExtractErr::Match)
        }
    }
}

impl<P: Parameter> Parameter for Option<P> {
    type Arg<'a> = Option<P::Arg<'a>>;

    fn regex() -> &'static Regex {
        P::regex()
    }

    fn usage_gr() -> String {
        return "[".to_string() + Self::usage() + "]"
    }

    fn usage() -> &'static str {
        P::usage()
    }

    fn completer() -> &'static dyn CommandCompleter {
        P::completer()
    }
}

impl<'a, A: Argument<'a>> Argument<'a> for Option<A> {
    fn extract<'b>(args: &'a str, from: usize) -> Result<(Self, usize), ExtractErr<'b>> {
        if from >= args.len() {
            return Ok((None, from))
        }

        Self::extract_something(args, from)
    }

    fn parse<'b>(arg: &'a str) -> Result<Self, &'b str> {
        match A::parse(arg) {
            Ok(arg) => Ok(Some(arg)),
            Err(e) => Err(e)
        }
    }
}

pub trait CommandCompleter {
    fn complete(&self, text: &str) -> Vec<String>;
}

pub enum ExecErr<'a> {
    TooFew,
    TooMany,
    Match,
    Parse(&'a str)
}

pub trait AbstractCommand<T> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), (usize, ExecErr<'a>)>;
    fn complete(&self, text: &str) -> (usize, Vec<String>);
    fn name(&self) -> &'static str;
    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error>;

    fn usage(&self, err: &mut dyn Write) {
        write!(err, "usage: {} ", self.name()).unwrap();
        self.param_usage(err).unwrap();
        writeln!(err).unwrap();
    }

    fn indicator(&self, err: &mut dyn Write, pos: usize, extra: usize) {
        writeln!(err, "{}^", String::from_utf8(vec![b' '; extra + self.name().len() + pos]).unwrap()).unwrap();
    }

    fn error(&self, err: &mut dyn Write, e: (usize, ExecErr), extra: usize) {
        self.indicator(err, e.0, extra);
        writeln!(err, "{}", match e.1 {
            ExecErr::TooFew => "Too few arguments.",
            ExecErr::TooMany => "Too many arguments.",
            ExecErr::Match => "Parse error.",
            ExecErr::Parse(reason) => reason,
        }).unwrap();
        self.usage(err);
    }
}

fn lws(text: &str) -> usize {
    text.len() - text.trim_start().len()
}

macro_rules! cmdimpl {
    ($param_count:expr) => {
        cmdimpl!($param_count, );
    };
    ($param_count:expr, $($param:ident),*) => {
        paste! {
            type [<CmdFn $param_count>]<T, $($param),*> = for <'a> fn(&mut T, $(&<$param as Parameter>::Arg<'a>),*);

            struct [<Cmd $param_count>]<T, $($param: Parameter),*> {
                name: &'static str,
                func: [<CmdFn $param_count>]<T, $($param),*>
            }

            impl <T, $($param: Parameter),*> [<Cmd $param_count>]<T, $($param),*> {
                pub fn new(name: &'static str, func: [<CmdFn $param_count>]<T, $($param),*>) -> Self {
                    Self{name, func}
                }
            }

            pub fn [<command $param_count>]<T: 'static, $($param: Parameter + 'static),*>(
                name: &'static str, func: [<CmdFn $param_count>]<T, $($param),*>, cmds: &mut Commands<T>)
            {
                cmds.insert(name, Box::new([<Cmd $param_count>]::new(name, func)));
            }

            impl <T, $($param: Parameter),*> AbstractCommand<T> for [<Cmd $param_count>]<T, $($param),*> {
                fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), (usize, ExecErr<'a>)> {
                    let mut next_at = lws(args);
                    let mut expect_next_at = 1;

                    $(
                        assert!(next_at <= args.len());

                        let ([<arg_ $param:lower>], arg_end) = match $param::Arg::extract(&args, next_at) {
                            Ok(result) => result,
                            Err(e) => return Err(match e {
                                ExtractErr::Missing => (expect_next_at, ExecErr::TooFew),
                                ExtractErr::Match => (next_at, ExecErr::Match),
                                ExtractErr::Parse(reason) => (next_at, ExecErr::Parse(reason))
                            })
                        };

                        let [<argref_ $param:lower>] = &[<arg_ $param:lower>];
                        next_at = arg_end + lws(&args[arg_end..]);
                        expect_next_at = arg_end + 1;
                    )*

                    //More arguments than expected is an error
                    if (next_at < args.len()) {
                        return Err((next_at, ExecErr::TooMany));
                    }

                    //Prevent compiler from whining when there are no parameters
                    let _ignore = (args, expect_next_at);
                    next_at = 0;
                    expect_next_at = 0;
                    let _ignore2 = (next_at, expect_next_at);

                    Ok((self.func)(processor, $([<argref_ $param:lower>],)* ))
                }

                fn complete(&self, args: &str) -> (usize, Vec<String>) {
                    let mut last_end = 0;

                    $(
                        let (arg_start, arg_end) = match $param::position(args, last_end) {
                            (0, 0) => return (last_end, $param::complete(args, last_end)),
                            pos => pos
                        };

                        assert!(arg_end <= args.len());

                        if arg_end == args.len() {
                            return (arg_start, $param::complete(args, last_end));
                        }

                        last_end = arg_end;
                    )*

                    //Prevent compiler from whining when there are no parameters
                    let _ignore = (args, last_end);
                    last_end = 0;
                    let _ignore2 = last_end;

                    (0, Vec::new())
                }

                fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error> {
                    $(
                        match write!(err, "{} ", $param::usage_gr()) {
                            Ok(()) => (),
                            Err(e) => return Err(e)
                        }
                    )*

                    let _ignore = err;
                    return Ok(())
                }

                fn name(&self) -> &'static str {
                    return self.name;
                }
            }
        }
    };
}

cmdimpl!(0);
cmdimpl!(1, P0);
cmdimpl!(2, P0, P1);
cmdimpl!(3, P0, P1, P2);

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
            Err(e) => command.error(self.stderr(), e, 2)
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

                /*
                 * "cmd    lol  gna"
                 *     ~~~~~~~~~~~~ <- args, len() = 11
                 *         ~~~~~~~~ <- args_trimmed, len() = 7
                 *              ^   <- argpos = 5
                 *
                 */

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
