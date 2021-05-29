use std::str::SplitWhitespace;
use std::marker::PhantomData;

type CommandFn<T> = fn(&mut T, &mut SplitWhitespace) -> bool;
pub struct Command<T> (pub &'static str, pub CommandFn<T>);

pub trait Interpreter {
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
        let optcmd = cmditer.find(|Command(n,_a)| n == &command); 
        
        match optcmd {
            None => false,
            Some(Command(_n, action)) => action(self, args)
        }
    }
    
    fn exec_fb(self : &mut Self, _command: &str, _args: &mut SplitWhitespace, _orig: &str)
        -> bool {false}
}

pub trait CommandsFB<'a> : Commands<'a> {
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

