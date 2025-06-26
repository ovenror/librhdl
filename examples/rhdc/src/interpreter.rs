extern crate paste;

use std::any::Any;
use std::collections::HashMap;
use std::io::Write;
use std::marker::PhantomData;
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

pub trait CompleterArgsContainer<CF: CompleterFactory> {
    const ARGS: CF::Args;
}

pub struct DefaultCompleterArgsContainer {}

impl<CF: CompleterFactory> CompleterArgsContainer<CF>
        for DefaultCompleterArgsContainer {
    const ARGS: CF::Args = CF::DEFAULT_ARGS;
}

pub trait Parameter : Sized {
    type Arg<'a> : Argument<'a>;
    type CF: CompleterFactory;
    const COMPLETER_ARGS: <<Self as Parameter>::CF as CompleterFactory>::Args = Self::CF::DEFAULT_ARGS;

    fn regex() -> &'static Regex;
    fn usage() -> &'static str;

    fn usage_gr() -> String {
        return "<".to_string() + Self::usage() + ">"
    }

    fn extract<'a, 'b>(args: &'a str, from: usize) -> Result<(&'a str, usize), ExtractErr<'b>>
    {
        if from >= args.len() {
            return Err(ExtractErr::Missing)
        }

        let args_from = &args[from..];

        assert!(lws(args_from) == 0);

        match Self::regex().captures(args_from) {
            Some(c) => {
                let m = c.get(0).unwrap();
                assert!(m.start() == 0);
                let extracted = args_from[..m.end()].trim_end();
                let extr_len = extracted.len();

                assert!(extr_len <= args_from.len());

                if extr_len == args_from.len() || args_from[extr_len..].chars().next().unwrap().is_whitespace() {
                    Ok((extracted, from + extr_len))
                } else {
                    Err(ExtractErr::Match)
                }
            },
            None => Err(ExtractErr::Match)
        }
    }

    fn ends_at(args: &str, from: usize) -> usize {
        match Self::extract(args, from) {
            Ok((_, end)) => end,
            Err(_) => 0
        }
    }
}

fn complete(completer: &dyn CommandCompleter, args: &str, from: usize) -> (usize, Vec<String>)
{
    let (pos, cand) = completer.complete(&args[from..]);
    (from + pos, cand)
}

pub enum ExtractErr<'a> {
    Missing,
    Match,
    Parse(&'a str)
}

pub trait Argument<'a> : Sized + Parameter
{
    fn parse_extracted<'b>(arg: &'a str) -> Result<Self, &'b str>;

    fn parse<'b>(args: &'a str, from: usize) -> Result<(Self, usize), ExtractErr<'b>>
    {
        match Self::extract(args, from) {
            Err(e) => Err(e),
            Ok((argstr, end)) => match Self::parse_extracted(argstr) {
                Ok(arg) => Ok((arg, end)),
                Err(e) => Err(ExtractErr::Parse(e))
            }
        }
    }
}

impl<P: Parameter> Parameter for Option<P> {
    type Arg<'a> = Option<P::Arg<'a>>;
    type CF = P::CF;

    fn regex() -> &'static Regex {
        P::regex()
    }

    fn usage_gr() -> String {
        return "[".to_string() + Self::usage() + "]"
    }

    fn usage() -> &'static str {
        P::usage()
    }
}

pub struct CompletionSpecializedParameter<
        P: Parameter,
        A: CompleterArgsContainer<CF> = DefaultCompleterArgsContainer,
        CF: CompleterFactory = <P as Parameter>::CF,
        > {
    phantom: PhantomData<(P, A, CF)>
}

impl<P: Parameter, A: CompleterArgsContainer<CF>, CF: CompleterFactory> Parameter
        for CompletionSpecializedParameter<P, A, CF>
{
    type Arg<'a> = P::Arg<'a>;
    type CF = CF;        
    const COMPLETER_ARGS: <<Self as Parameter>::CF as CompleterFactory>::Args = A::ARGS;

    fn regex() -> &'static Regex {
        P::regex()
    }

    fn usage() -> &'static str {
        P::usage()
    }
}

impl<'a, A: Argument<'a>> Argument<'a> for Option<A> {
    fn parse<'b>(args: &'a str, from: usize) -> Result<(Self, usize), ExtractErr<'b>> {
        match Self::extract(args, from) {
            Ok((argstr, end)) => match Self::parse_extracted(argstr) {
                Err(e) => Err(ExtractErr::Parse(e)),
                Ok(arg) => Ok((arg, end))
            }
            Err(ExtractErr::Missing) => Ok((None, from)),
            Err(e) => Err(e)
        }
    }

    fn parse_extracted<'b>(arg: &'a str) -> Result<Self, &'b str> {
        match A::parse_extracted(arg) {
            Ok(arg) => Ok(Some(arg)),
            Err(e) => Err(e)
        }
    }
}
pub trait CompleterFactory : 'static {
    type Args : Any + std::cmp::Eq + std::hash::Hash + Clone + 'static;
    const DEFAULT_ARGS : Self::Args;
    type Completer : CommandCompleter;

    fn create(args: &Self::Args) -> Self::Completer;

    fn id() -> std::any::TypeId
    {
        std::any::TypeId::of::<Self>()
    }
}


trait AbstractCompleterInstances<'a> {
    fn ensure_exists(&mut self, args: &dyn Any);
    fn get(&self, args: &dyn Any) -> &dyn CommandCompleter;
}

struct CompleterInstances<F: CompleterFactory> {
    completers: HashMap<F::Args, F::Completer>
}

impl<F: CompleterFactory> CompleterInstances<F> {
    fn new() -> Self {
        Self {completers: HashMap::new()}
    }
}

impl<'a, F: CompleterFactory> AbstractCompleterInstances<'a> for CompleterInstances<F> {
    fn ensure_exists(&mut self, args: &dyn Any) {
        let typed_args = args.downcast_ref::<F::Args>().unwrap();

        if self.completers.contains_key(typed_args) {
            return
        }

        self.completers.insert(typed_args.clone(), F::create(typed_args));
    }

    fn get(&self, args: &dyn Any) -> &dyn CommandCompleter {
        let typed_args = args.downcast_ref::<F::Args>().unwrap();
        self.completers.get(typed_args).unwrap()
    }
}

pub struct CompleterManager<'a> {
    completers: HashMap<std::any::TypeId, Box<dyn AbstractCompleterInstances<'a> + 'a>>
}

impl<'a> CompleterManager<'a> {
    pub fn new() -> Self {
        Self {completers: HashMap::new()}
    }

    fn ensure_exists<F: CompleterFactory>(&mut self, args: &F::Args) {
        let factory_id = F::id();

        if !self.completers.contains_key(&factory_id) {
            let ci = CompleterInstances::<F>::new();
            self.completers.insert(factory_id, Box::new(ci));
        }

        self.completers.get_mut(&factory_id).unwrap().ensure_exists(args);
    }


    fn get<F: CompleterFactory>(&self, args: &F::Args) -> &dyn CommandCompleter {
        let factory_id = F::id();
        self.completers.get(&factory_id).unwrap().get(args)
    }
}

pub trait CommandCompleter {
    fn complete(&self, text: &str) -> (usize, Vec<String>);
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

pub fn lws(text: &str) -> usize {
    text.len() - text.trim_start().len()
}

trait AbstractCommandBuilder<'a, 'b: 'a, T: 'b> {
    fn ensure_completers(&self, cm: &mut CompleterManager);
    fn build<'c: 'b>(&self, cm: &'b CompleterManager<'c>) -> Box<dyn AbstractCommand<T> + 'b>;
}

pub struct CommandsBuilder<'a, 'b: 'a, T: 'b> {
    builders: Vec<Box<dyn AbstractCommandBuilder<'a, 'b, T> + 'a>>
}

impl<'a, 'b: 'a, T: 'b> CommandsBuilder<'a, 'b, T> {
    pub fn new() -> Self
    {
        Self {builders: Vec::new()}
    }

    pub fn ensure_completers(&self, cm: &mut CompleterManager)
    {
        for builder in self.builders.iter() {
            builder.as_ref().ensure_completers(cm)
        }
    }

    pub fn get_commands<'c: 'b>(&self, cm: &'b CompleterManager<'c>) -> Commands<'b, T>
    {
        let mut commands = Commands::new();

        for builder in self.builders.iter() {
            let cmd = builder.as_ref().build(cm);
            commands.insert(cmd.name(), cmd);
        }

        commands
    }
}

macro_rules! cmdimpl {
    ($param_count:expr) => {
        cmdimpl!($param_count, );
    };
    ($param_count:expr, $($param:ident),*) => {
        paste! {
            type [<CmdFn $param_count>]<T, $($param),*> = for <'a> fn(&mut T, $(&<$param as Parameter>::Arg<'a>),*);

            struct [<Cmd $param_count>]<'a, T, $($param: Parameter),*> {
                name: &'static str,
                func: [<CmdFn $param_count>]<T, $($param),*>,
                phantom: PhantomData<&'a bool>,
                $([<completer $param:lower>]: &'a dyn CommandCompleter),*
            }

            impl <'a, T, $($param: Parameter),*> [<Cmd $param_count>]<'a, T, $($param),*> {
                pub fn new(
                        name: &'static str,
                        func: [<CmdFn $param_count>]<T, $($param),*>,
                        $([<completer $param:lower>]: &'a dyn CommandCompleter),*) -> Self
                {
                    Self{name, func, phantom: PhantomData, $([<completer $param:lower>]),*}
                }
            }

            struct [<CmdBuilder $param_count>]<T, $($param: Parameter),*> {
                name: &'static str,
                func: [<CmdFn $param_count>]<T, $($param),*>
            }

            impl<T, $($param: Parameter),*> [<CmdBuilder $param_count>]<T, $($param),*> {
                fn new(name: &'static str, func: [<CmdFn $param_count>]<T, $($param),*>) -> Self {
                    Self {name, func}
                }
            }

            impl<'a, 'b: 'a, T: 'b, $($param: Parameter + 'b),*> AbstractCommandBuilder<'a, 'b, T>
                    for [<CmdBuilder $param_count>]<T, $($param),*>
            {
                fn ensure_completers(&self, cm: &mut CompleterManager)
                {
                    let _ignore = &cm;
                    $(cm.ensure_exists::<$param::CF>(&$param::COMPLETER_ARGS);)*
                }

                fn build<'c: 'b>(&self, cm: &'b CompleterManager<'c>) -> Box<dyn AbstractCommand<T> + 'b>
                {
                    let _ignore = &cm;
                    Box::new([<Cmd $param_count>]::new(
                        self.name, self.func, $(cm.get::<$param::CF>(&$param::COMPLETER_ARGS)),*))
                }
            }

            impl<'a, 'b: 'a, T: 'b> CommandsBuilder<'a, 'b, T> {
                pub fn [<command $param_count>]<$($param: Parameter + 'static),*> (
                        &mut self,
                        name: &'static str,
                        func: [<CmdFn $param_count>]<T, $($param),*>)
                {
                    self.builders.push(Box::new([<CmdBuilder $param_count>]::<T, $($param),*>::new(name, func)));
                }
            }

            impl <'a, T, $($param: Parameter),*> AbstractCommand<T> for [<Cmd $param_count>]<'a, T, $($param),*> {
                fn exec<'b>(&self, processor: &mut T, args: &str) -> Result<(), (usize, ExecErr<'b>)> {
                    let mut next_at = lws(args);
                    let mut expect_next_at = 1;

                    $(
                        assert!(next_at <= args.len());

                        let ([<arg_ $param:lower>], arg_end) = match $param::Arg::parse(&args, next_at) {
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
                    let mut next_at = lws(args);

                    $(
                        let arg_end = match $param::ends_at(args, next_at) {
                            0 => return complete(self.[<completer $param:lower>], args, next_at),
                            pos => pos
                        };

                        assert!(arg_end <= args.len());

                        if arg_end == args.len() {
                            return complete(self.[<completer $param:lower>], args, next_at);
                        }

                        next_at = arg_end + lws(&args[arg_end..]);
                    )*

                    //Prevent compiler from whining when there are no parameters
                    let _ignore = (args, next_at);
                    next_at = 0;
                    let _ignore2 = next_at;

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
pub type Commands<'a, T> = HashMap<&'static str, Box<dyn AbstractCommand<T> + 'a>>;


pub trait Processor : Sized {
    type Fallback : Interpreter<Candidate = Pair>;

    fn commands(cb: &mut CommandsBuilder<Self>);
    fn fallback_mut(&mut self) -> &mut Self::Fallback;
    fn fallback(&self) -> &Self::Fallback;
    fn stderr(&mut self) -> &mut dyn Write;
    fn interactive(&self) -> bool;
    fn prompt_size(&self) -> usize;

    fn exec(&mut self, cmds: &Commands<Self>, cmd: &str, args: &str, orig: &String) -> bool {
        let command = match cmds.get(cmd) {
            Some(c) => c.as_ref(),
            None => return self.exec_fb(cmd, args, orig)
        };

        match command.exec(self, args) {
            Ok(_) => (),
            Err(e) => {
                let prompt_size = if self.interactive() {
                    self.prompt_size()
                } else {
                    writeln!(self.stderr(), "{}", orig).unwrap();
                    0
                };

                command.error(self.stderr(), e, prompt_size)
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

    fn complete_object_contextually(&self, line: &str) -> (usize, Vec<String>);

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
                            filter(|cmd| cmd.starts_with(&line_trimmed))
                            .map(|cmd| {
                                let (_, repl) = cmd.split_at(pos + line_trimmed.len() - line.len());
                                Pair{
                                    display: cmd.to_string(),
                                    replacement: repl.to_string()
                                }
                            })
                            .collect();
 
                    return Ok((pos, cmds.to_vec()))
                }
            }
        };

        let optcmd = cmds.get(command); 
        
        match optcmd {
            Some(cmd) => {
                let cursor_pos_in_line_trimmed = pos + line.len() - line_trimmed.len();
                let args_lws = lws(args);
                let cursor_pos_in_args = cursor_pos_in_line_trimmed + args.len() - line_trimmed.len();
                let to_complete = &args[args_lws..cursor_pos_in_args];
                let (argcand_pos, mut argcand) = cmd.complete(to_complete);

                /* FIXME: Condition should be a member of (Abstract)Command */
                let (allcand_pos, allcand) = if command == "ls" {
                    let (ctxcand_pos, mut ctxcand) = self.complete_object_contextually(to_complete);
                    //dbg!(&argcand);
                    //dbg!(&ctxcand);
                    assert!(ctxcand.len() == 0 || argcand.len() == 0 || argcand_pos == ctxcand_pos);

                    if ctxcand.is_empty() {
                        (argcand_pos, argcand)
                    } else {
                        argcand.append(&mut ctxcand);
                        (ctxcand_pos, argcand)
                    }
                } else {
                    (argcand_pos, argcand)
                };

                let arglen_upto_cursor = to_complete.len() - allcand_pos;

                /*            1         2
                 *  01234567890123456789012
                 *
                 * "  cmd    lol  hmpf.gnarf bla"
                 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~      <- line, len() = 28
                 *  ⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻^           <- pos = 22 (cursor position)
                 *    ~~~~~~~~~~~~~~~~~~~~~~~~~~      <- line_trimmed, len() = 26
                 *    ⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻^           <- cursor_pos_in_line_trimmed = 20
                 *       ~~~~~~~~~~~~~~~~~~~~~~~      <- args, len() = 23
                 *       ⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻^           <- cursor_pos_in_args = 17
                 *       ~~~~                         <- args_lws = 4
                 *           ~~~~~~~~~~~~~            <- to_complete, len() = 13
                 *           ⁻⁻⁻⁻⁻⁻⁻⁻⁻⁻^              <- argcand_pos = 10
                 *                    "gnaftl"        <- argcand[0]
                 *                     ~~~            <- arglen_upto_cursor = 3
                 *                       "ftl"        <- argcand[0][arglen_upto_cursor..]
                 */

                let result: Vec<Pair> = allcand.iter().
                        map(|arg| Pair {
                                display: arg.to_string(),
                                replacement: arg[arglen_upto_cursor..].to_string()}).
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

pub struct SimpleInterpreter<'a, C: Processor> {
    processor: C,
    commands: Commands<'a, C>,
}

impl<'a, C: Processor> SimpleInterpreter<'a, C> {
    pub fn new<'b: 'a>(processor: C, cb: CommandsBuilder<'_, 'a, C>, cm: &'a CompleterManager<'b>) -> Self {
    //pub fn new(processor: C, commands: Commands<'a, C>) -> Self {
        let commands = cb.get_commands(cm);
        Self {processor, commands}
    }

    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool {
        self.processor.exec(&self.commands, cmd, args, orig)
    }

    pub fn get_processor(&self) -> &C {
        &self.processor
    }
}

impl<'a, C : Processor> Interpreter for SimpleInterpreter<'a, C>
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

impl<'a, C : Processor> Completer for SimpleInterpreter<'a, C> {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let (p1, mut vec1) = match self.processor.complete(&self.commands, line, pos, ctx) {
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

        assert!(p1 == p2);
        vec1.append(&mut vec2);
        return Ok((p1, vec1))
    }
}
