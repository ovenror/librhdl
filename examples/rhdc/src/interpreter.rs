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

pub trait ParameterArgsContainer<P: Parameter> {
    const ARGS: P::CreationArguments;
}

pub struct DefaultParameterArgsContainer {}

impl<P: Parameter> ParameterArgsContainer<P>
        for DefaultParameterArgsContainer {
    const ARGS: P::CreationArguments = P::ARGS;
}

pub trait Parameter : Any + Sized {
    type Argument<'a>;
    type CreationArguments;
    const ARGS: Self::CreationArguments;

    fn create(args: Self::CreationArguments) -> Self;
    fn regex() -> &'static Regex;
    fn usage() -> &'static str;
    fn parse_extracted<'a, 'b>(&self, arg: &'a str) -> Result<Self::Argument<'a>, String>;
    fn completer(&self) -> &dyn CommandCompleter;

    fn new() -> Self {
        Self::create(Self::ARGS)
    }

    fn id() -> std::any::TypeId
    {
        std::any::TypeId::of::<Self>()
    }

    fn parse<'a>(&self, args: &'a str, from: usize) -> Result<(Self::Argument<'a>, usize), ExtractErr>
    {
        match Self::extract(args, from) {
            Err(e) => Err(e),
            Ok((argstr, end)) => match self.parse_extracted(argstr) {
                Ok(arg) => Ok((arg, end)),
                Err(e) => Err(ExtractErr::Parse(e))
            }
        }
    }

    fn usage_gr() -> String {
        return "<".to_string() + Self::usage() + ">"
    }

    fn extract<'a>(args: &'a str, from: usize) -> Result<(&'a str, usize), ExtractErr>
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

    fn complete(&self, args: &str, from: usize) -> (usize, Vec<String>)
    {
        let (pos, cand) = self.completer().complete(&args[from..]);
        (from + pos, cand)
    }

}
pub struct ArgsSpecializedParameter<P: Parameter, A: ParameterArgsContainer<P>> {
    parameter: P,
    phantom: PhantomData<A>
}

impl<P: Parameter, A: ParameterArgsContainer<P> + 'static> Parameter for ArgsSpecializedParameter<P, A> {
    type Argument<'a> = P::Argument<'a>;
    type CreationArguments = P::CreationArguments;
    const ARGS: Self::CreationArguments = A::ARGS;

    #[inline]
    fn create(args: Self::CreationArguments) -> Self {
        Self {parameter: P::create(args), phantom: PhantomData}
    }

    #[inline]
    fn regex() -> &'static Regex {
        P::regex()
    }

    #[inline]
    fn usage() -> &'static str {
        P::usage()
    }

    fn parse_extracted<'a>(&self, arg: &'a str) -> Result<Self::Argument<'a>, String> {
        self.parameter.parse_extracted(arg)
    }

    fn completer(&self) -> &dyn CommandCompleter {
        self.parameter.completer()
    }
}

pub enum ExtractErr {
    Missing,
    Match,
    Parse(String)
}

pub struct OptionalParameter<P: Parameter> {
    p: P
}

impl<P: Parameter> Parameter for OptionalParameter<P> {
    type Argument<'a> = Option<P::Argument<'a>>;
    type CreationArguments = P::CreationArguments;
    const ARGS: Self::CreationArguments = P::ARGS;

    fn create(args: Self::CreationArguments) -> Self {
        Self {p: P::create(args)}
    }

    #[inline]
    fn regex() -> &'static Regex {
        P::regex()
    }

    #[inline]
    fn usage() -> &'static str {
        P::usage()
    }

    fn usage_gr() -> String {
        return "[".to_string() + Self::usage() + "]"
    }

    fn parse<'a>(&self, args: &'a str, from: usize)
            -> Result<(Self::Argument<'a>, usize), ExtractErr>
    {
        match Self::extract(args, from) {
            Ok((argstr, end)) => match self.parse_extracted(argstr) {
                Err(e) => Err(ExtractErr::Parse(e)),
                Ok(arg) => Ok((arg, end))
            }
            Err(ExtractErr::Missing) => Ok((None, from)),
            Err(e) => Err(e)
        }
    }

    fn parse_extracted<'a>(&self, arg: &'a str) -> Result<Self::Argument<'a>, String> {
        match self.p.parse_extracted(arg) {
            Ok(arg) => Ok(Some(arg)),
            Err(e) => Err(e)
        }
    }

    fn completer(&self) -> &dyn CommandCompleter {
        self.p.completer()
    }
}


pub struct ParameterManager {
    instances: HashMap<std::any::TypeId, Box<dyn Any>>
}

impl ParameterManager {
    pub fn new() -> Self {
        let mut map = HashMap::<std::any::TypeId, Box<dyn Any>>::new();
        map.insert(Command::id(), Box::new(Command::new()));
        Self {instances: HashMap::new()}
    }

    fn ensure_exists<P: Parameter>(&mut self) {
        let id = P::id();

        if !self.instances.contains_key(&id) {
            self.instances.insert(id, Box::new(P::new()));
        }
    }

    pub fn get<P: Parameter>(&self) -> &P {
        let factory_id = P::id();
        self.instances.get(&factory_id).unwrap().downcast_ref::<P>().unwrap()
    }
}

pub trait CommandCompleter {
    fn complete(&self, text: &str) -> (usize, Vec<String>);
}

pub enum NoParseExecErr {
    TooFew,
    TooMany,
    Match,
}

pub enum ExecErr {
    NoParse(NoParseExecErr),
    Parse(String),
}

pub trait AbstractCommand<T> {
    fn exec<'a>(&self, processor: &mut T, args: &str) -> Result<(), (usize, ExecErr)>;
    fn complete(&self, text: &str) -> (usize, Vec<String>);
    fn name(&self) -> &'static str;
    fn param_usage(&self, err: &mut dyn Write) -> Result<(), std::io::Error>;
    fn help_short(&self) -> &'static str;

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
        match e.1 {
            ExecErr::NoParse(np) => writeln!(err, "{}", match np {
                NoParseExecErr::TooFew => "Too few arguments.",
                NoParseExecErr::TooMany => "Too many arguments.",
                NoParseExecErr::Match => "Parse error.",
            }),
            ExecErr::Parse(reason) => writeln!(err, "{}", reason),
        }.unwrap();
        self.usage(err);
    }
}

pub fn lws(text: &str) -> usize {
    text.len() - text.trim_start().len()
}

trait AbstractCommandBuilder<'a, 'b: 'a, T: 'b> {
    fn ensure_parameters(&self, pm: &mut ParameterManager);
    fn build<'c: 'b>(&self, pm: &'b ParameterManager) -> Box<dyn AbstractCommand<T> + 'b>;
}

pub struct CommandsBuilder<'a, 'b: 'a, T: 'b> {
    builders: Vec<Box<dyn AbstractCommandBuilder<'a, 'b, T> + 'a>>
}

impl<'a, 'b: 'a, T: 'b> CommandsBuilder<'a, 'b, T> {
    pub fn new() -> Self
    {
        Self {builders: Vec::new()}
    }

    pub fn ensure_parameters(&self, pm: &mut ParameterManager)
    {
        for builder in self.builders.iter() {
            builder.as_ref().ensure_parameters(pm)
        }
    }

    pub fn get_commands<'c: 'b>(&self, pm: &'b ParameterManager) -> Commands<'b, T>
    {
        let mut commands = Commands::new();

        for builder in self.builders.iter() {
            let cmd = builder.as_ref().build(pm);
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
            type [<CmdFn $param_count>]<T, $($param),*> = for <'a> fn(&mut T, $(&<$param as Parameter>::Argument<'a>),*);

            struct [<Cmd $param_count>]<'a, T, $($param: Parameter),*> {
                name: &'static str,
                func: [<CmdFn $param_count>]<T, $($param),*>,
                help_short: &'static str,
                phantom: PhantomData<&'a bool>,
                $([<$param:lower>]: &'a $param),*
            }

            impl <'a, T, $($param: Parameter),*> [<Cmd $param_count>]<'a, T, $($param),*> {
                pub fn new(
                        name: &'static str,
                        func: [<CmdFn $param_count>]<T, $($param),*>,
                        help_short: &'static str,
                        $([<$param:lower>]: &'a $param),*) -> Self
                {
                    Self{
                        name, func, help_short, phantom: PhantomData,
                        $([<$param:lower>]),*
                    }
                }
            }

            struct [<CmdBuilder $param_count>]<T, $($param: Parameter),*> {
                name: &'static str,
                func: [<CmdFn $param_count>]<T, $($param),*>,
                help_short: &'static str,
            }

            impl<T, $($param: Parameter),*> [<CmdBuilder $param_count>]<T, $($param),*> {
                fn new(name: &'static str, func: [<CmdFn $param_count>]<T, $($param),*>, help_short: &'static str) -> Self {
                    Self {name, func, help_short}
                }
            }

            impl<'a, 'b: 'a, T: 'b, $($param: Parameter + 'b),*> AbstractCommandBuilder<'a, 'b, T>
                    for [<CmdBuilder $param_count>]<T, $($param),*>
            {
                fn ensure_parameters(&self, pm: &mut ParameterManager)
                {
                    let _ignore = &pm;
                    $(pm.ensure_exists::<$param>();)*
                }

                fn build<'c: 'b>(&self, pm: &'b ParameterManager) -> Box<dyn AbstractCommand<T> + 'b>
                {
                    let _ignore = &pm;
                    Box::new([<Cmd $param_count>]::new(
                        self.name, self.func, self.help_short,
                        $(pm.get::<$param>()),*))
                }
            }

            impl<'a, 'b: 'a, T: 'b> CommandsBuilder<'a, 'b, T> {
                pub fn [<command $param_count>]<$($param: Parameter + 'b),*> (
                        &mut self,
                        name: &'static str,
                        func: [<CmdFn $param_count>]<T, $($param),*>,
                        help_short: &'static str)
                {
                    self.builders.push(Box::new(
                        [<CmdBuilder $param_count>]::<T, $($param),*>::new(
                            name, func, help_short)));
                }
            }

            impl <'a, T, $($param: Parameter),*> AbstractCommand<T> for [<Cmd $param_count>]<'a, T, $($param),*> {
                fn exec(&self, processor: &mut T, args: &str) -> Result<(), (usize, ExecErr)> {
                    let mut next_at = lws(args);
                    let mut expect_next_at = 1;

                    $(
                        assert!(next_at <= args.len());

                        let ([<arg_ $param:lower>], arg_end) = match self.[<$param:lower>].parse(&args, next_at) {
                            Ok(result) => result,
                            Err(e) => return Err(match e {
                                ExtractErr::Missing => (expect_next_at, ExecErr::NoParse(NoParseExecErr::TooFew)),
                                ExtractErr::Match => (next_at, ExecErr::NoParse(NoParseExecErr::Match)),
                                ExtractErr::Parse(reason) => (next_at, ExecErr::Parse(reason)),
                            })
                        };

                        let [<argref_ $param:lower>] = &[<arg_ $param:lower>];
                        next_at = arg_end + lws(&args[arg_end..]);
                        expect_next_at = arg_end + 1;
                    )*

                    //More arguments than expected is an error
                    if (next_at < args.len()) {
                        return Err((next_at, ExecErr::NoParse(NoParseExecErr::TooMany)));
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
                            0 => return self.[<$param:lower>].complete(args, next_at),
                            pos => pos
                        };

                        assert!(arg_end <= args.len());

                        if arg_end == args.len() {
                            return self.[<$param:lower>].complete(args, next_at);
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

                fn help_short(&self) -> &'static str {
                    return self.help_short;
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

struct CmdCompleter {
    cmds: Vec<&'static str>
}

impl CmdCompleter {
    fn new() -> Self {
        Self {cmds: Vec::new()}
    }

    fn add(&mut self, cmds: Vec<&'static str>) {
        self.cmds.extend(cmds.into_iter());
    }
}

impl CommandCompleter for CmdCompleter {
    fn complete(&self, text: &str) -> (usize, Vec<String>) {
        let candidates: Vec<String> = self.cmds.iter().
                filter(|cmd| cmd.starts_with(text)).
                map(|cmd| cmd.to_string()).
                collect();
        (0, candidates)
    }
}

pub struct Command {
    completer: CmdCompleter
}

impl Command {
    fn new() -> Self {
        Self {completer: CmdCompleter::new()}
    }

    fn add(&mut self, cmds: Vec<&'static str>) {
        self.completer.add(cmds);
    }
}

impl Parameter for Command {
    type Argument<'a> = &'a str;
    type CreationArguments = Vec<&'static str>;
    const ARGS: Self::CreationArguments = vec![];

    fn create(args: Self::CreationArguments) -> Self {
        Command {completer: CmdCompleter {cmds: args}}
    }

    fn regex() -> &'static Regex {
        &REGEX_CMDLINE
    }

    fn usage() -> &'static str {
        "command"
    }

    fn parse_extracted<'a, 'b>(&self, arg: &'a str) -> Result<Self::Argument<'a>, String> {
        Ok(arg)
    }

    fn completer(&self) -> &dyn CommandCompleter {
        &self.completer
    }
}

pub trait Processor : Sized {
    type Fallback : Interpreter<Candidate = Pair>;

    fn commands(cb: &mut CommandsBuilder<Self>);
    fn fallback_mut(&mut self) -> &mut Self::Fallback;
    fn fallback(&self) -> &Self::Fallback;
    fn stderr(&mut self) -> &mut dyn Write;
    fn interactive(&self) -> bool;
    fn prompt_size(&self) -> usize;

    fn help_list_fb(&mut self) {}

    fn get_fallback_cmd_names(&self) -> Vec<&'static str>
    {
        Vec::new()
    }

    fn help_dummy(&mut self, _cmd: &Option<&str>) {}

    fn help(&mut self, cmd: &str, cmds: &Commands<Self>) {
        if cmd.is_empty() {
            self.help_overview(cmds);
        } else {
            self.help_command(cmd, cmds);
        }
    }

    fn help_command(&mut self, cmd: &str, cmds: &Commands<Self>) {
        match cmds.get(cmd) {
            Some(c) => {
                c.usage(self.stderr());
            }
            None => {
                writeln!(self.stderr(), "No such command: {}", cmd).unwrap();
                writeln!(self.stderr(), "Type 'help' for a list of commands.").unwrap();
            }
        }
    }

    fn help_overview(&mut self, cmds: &Commands<Self>) {
        writeln!(self.stderr(), "Available commands:").unwrap();
        self.help_list(cmds);
        self.help_list_fb();
        writeln!(self.stderr(), "Type 'help <command>' for details.").unwrap();
    }

    fn help_list(&mut self, cmds: &Commands<Self>) {
        cmds.iter().for_each(|(_, cmd)| {
            writeln!(self.stderr(), "{:20} {}", cmd.name(), cmd.help_short()).unwrap();
        });
    }

    fn exec(&mut self, cmds: &Commands<Self>, cmd: &str, args: &str, orig: &String) -> bool {
        let command = match cmds.get(cmd) {
            Some(c) =>
                if cmd == "help" {
                    self.help(args.trim(), cmds); return true;
                } else {
                    c.as_ref()
                },
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
        let line_lws = line.len() - line_trimmed.len();
        let cursor_pos_in_line_trimmed = pos - line_lws;

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
                                let (_, repl) = cmd.split_at(cursor_pos_in_line_trimmed);
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
                 *  ~~                                <- line_lws = 2
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
    commands: Commands<'a, C>
}

impl<'a, C: Processor> SimpleInterpreter<'a, C> {
    pub fn new<'b: 'a>(processor: C, cb: CommandsBuilder<'_, 'a, C>, cm: &'a ParameterManager) -> Self {
    //pub fn new(processor: C, commands: Commands<'a, C>) -> Self {
        let commands = cb.get_commands(cm);
        Self {processor, commands}
    }

    pub fn mk_help_param() -> OptionalParameter<Command> {
        OptionalParameter::<Command>::new()
    }

    pub fn new_with_help<'b: 'a>(
        processor: C, cb: CommandsBuilder<'_, 'a, C>, cm: &'a ParameterManager,
        help_param: &'a mut OptionalParameter<Command>) -> Self
    {
        let mut instance = Self::new(processor, cb, cm);

        help_param.p.add(vec!["help"]);
        help_param.p.add(instance.get_commands().keys().cloned().collect());
        help_param.p.add(instance.processor.get_fallback_cmd_names());

        dbg!(&help_param.p.completer.cmds);

        instance.commands.insert("help", Box::new(Cmd1::<'a, C, OptionalParameter<Command>>::new(
            "help", C::help_dummy, "Show this help", help_param)));

        instance
    }

    fn exec(&mut self, cmd: &str, args: &str, orig: &String) -> bool {
        self.processor.exec(&self.commands, cmd, args, orig)
    }

    pub fn get_processor(&self) -> &C {
        &self.processor
    }

    pub fn get_commands(&self) -> &Commands<'a, C> {
        &self.commands
    }

    pub fn help_list(&mut self) {
        self.processor.help_list(&self.commands);
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
