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

    fn exec(self : &mut Self, _command: &str, _args: &mut SplitWhitespace, _orig: &String) -> bool
    {
        true
    }
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
    
    fn exec_fb(self : &mut Self, _command: &str, _args: &mut SplitWhitespace, _orig: &String)
        -> bool {false}

    fn complete_object_contextually(&self, line: &str) -> Vec<String>;

    fn complete(&self, line: &str, pos: usize, _ctx: &Context<'_>)
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
                    let cmditer2 = Self::COMMANDS.into_iter();
                    let cmds : Vec<Pair> = cmditer2.
                            map(|Command(n,_a, _c)| n).
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

        let mut cmditer = Self::COMMANDS.into_iter();
        let optcmd = cmditer.find(|Command(n,_a, _c)| n == &command); 
        
        match optcmd {
            Some(Command(name, _action, completer)) => {
                let args_trimmed = args.trim_start();
                let mut argcand = completer.complete(args_trimmed);

                /* FIXME: Condition should be a member of Command */
                if name == &"ls" {
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

    fn crop_candidate(c: &Pair, by: usize) -> Pair {
        let (_, d) = c.display.split_at(by);
        let (_, r) = c.replacement.split_at(by);

        Pair{display: d.to_string(), replacement: r.to_string()}
    }

    fn crop_candidates(v: &Vec<Pair>, by: usize) -> Vec<Pair> {
        v.into_iter().map(|c| Self::crop_candidate(c, by)).collect()
    }
}

impl<'a, C : Commands<'a>> Interpreter for SimpleInterpreter<'a, C>
{
    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &String)
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
        let (mut p1, mut vec1) = match self.commands.complete(line, pos, ctx) {
            Ok(result) => result,
            _ => (0, Vec::<Pair>::new())
        };
        
        let (p2, mut vec2) = match self.commands.complete_fb(line, pos, ctx) {
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
