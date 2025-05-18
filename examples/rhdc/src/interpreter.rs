use std::str::SplitWhitespace;
use std::marker::PhantomData;

use rustyline::Context;
use rustyline::completion::{Completer,Pair};
use rustyline::error::ReadlineError;


pub trait CommandCompleter {
    fn complete(&self, text: &str) -> Vec<String>;
}

type CommandFn<T> = fn(&mut T, &mut SplitWhitespace) -> bool;
pub struct Command<T> (
    pub &'static str, pub CommandFn<T>, pub &'static dyn CommandCompleter);

pub trait Interpreter : Completer {
    fn eat(self : &mut Self, line : &String) -> bool {
        let mut args = line.trim().split_whitespace();
        let command = args.next().unwrap_or("");

        return self.exec(command, &mut args, line);
    }

    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str)
        -> bool; 
}

pub trait Commands<'a> : Sized where Self:'a {
    const COMMANDS: &'a [Command<Self>];
    
    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace)
        -> bool 
    {
        let mut cmditer = Self::COMMANDS.into_iter();
        let optcmd = cmditer.find(|Command(n,_a, _c)| n == &command); 
        
        match optcmd {
            None => false,
            Some(Command(_n, action, _completer)) => action(self, args)
        }
    }
    
    fn exec_fb(self : &mut Self, _command: &str, _args: &mut SplitWhitespace, _orig: &str)
        -> bool {false}

    fn complete(&self, line: &str, pos: usize, _ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>
    {
        let line_trimmed = line.trim();
        let (command, args) = match line_trimmed.split_once(' ') {
            Some((c, a)) => (c,a),
            None => {
                if line.ends_with(' ') {
                    (line_trimmed, "")
                }
                else {
                    let cmditer2 = Self::COMMANDS.into_iter();
                    let cmds : Vec<Pair> = cmditer2.
                            map(|Command(n,_a, _c)| n).
                            filter(|cmd| cmd.starts_with(&line)).
                            map(|cmd| {let (_, last) = cmd.split_at(pos); last}).
                            //map(|cmd| cmd.to_string()).
                            map(|cmd| Pair{
                                display: cmd.to_string(),
                                replacement: cmd.to_string()})
                            .collect();
 
                    return Ok((0, cmds.to_vec()))
                }
            }
        };

        let mut cmditer = Self::COMMANDS.into_iter();
        let optcmd = cmditer.find(|Command(n,_a, _c)| n == &command); 
        
        match optcmd {
            Some(Command(_n, _action, completer)) => {
                let args_trimmed = args.trim();
                let veciter = completer.complete(args_trimmed).into_iter();
                let result: Vec<Pair> = veciter.
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

        return Ok((pos, Vec::<Pair>::new()))
    }
    
    fn complete_fb(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Pair>), ReadlineError>;
}

pub struct SimpleInterpreter<'a, C : Commands<'a>> {
    commands : C,
    phantom : PhantomData<&'a Self> 
}

impl<'a, C: Commands<'a>> SimpleInterpreter<'a, C> {
    pub fn new(commands : C) -> SimpleInterpreter<'a, C> {
        SimpleInterpreter {commands : commands, phantom: PhantomData}
    }

    pub fn get_commands(&self) -> &C {
        &self.commands
    }
}

impl<'a, C : Commands<'a>> Interpreter for SimpleInterpreter<'a, C> {
    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str)
        -> bool 
    {
        if self.commands.exec(command, args) {
            return true;
        }

        self.commands.exec_fb(command, args, orig)
    }
}

impl<'a, C : Commands<'a>> Completer for SimpleInterpreter<'a, C> {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let mut vec = match self.commands.complete(line, pos, ctx) {
            Ok((_, v)) => v,
            _ => Vec::<Pair>::new()
        };
        
        let mut vec2 = match self.commands.complete_fb(line, pos, ctx) {
            Ok((_, v)) => v,
            _ => Vec::<Pair>::new()
        };

        vec.append(&mut vec2);
        return Ok((pos, vec))
    }
}
