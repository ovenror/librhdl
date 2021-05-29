use std::str::SplitWhitespace;
use std::io;
use std::io::Write;
use std::process::exit;

use crate::interpreter;
use crate::interpreter::SimpleInterpreter;

use crate::wod::WriteOrDie;
use crate::stdemerg::StdEmerg;

pub struct Outputs {
    pub interactive: bool,
    pub out: Box<dyn Write>,
    pub err: Box<dyn Write>
}

impl Outputs {
    pub fn new(interactive: bool) -> Outputs {
        Outputs {
            interactive: interactive,
            out : if interactive
                    {Box::new(WriteOrDie::new(Box::new(std::io::stdout())))} else
                    {Box::new(std::io::sink())},
            err : if interactive
                    {Box::new(WriteOrDie::new(Box::new(std::io::stderr())))} else
                    {Box::new(StdEmerg::new())}
        }
    }
}

impl std::clone::Clone for Outputs {
    fn clone(&self) -> Outputs {
        Outputs::new(self.interactive)
    }
}

pub trait Commands<'a> : interpreter::Commands<'a> {
    fn prompt_info(&self) -> &str {
        ""
    }
}

pub trait AutoCommands<'a> : Commands<'a> {
    fn auto_new(outputs: Outputs) -> Self; 
}

pub trait ConsoleInterpreter : interpreter::Interpreter {
    fn prompt_info(&self) -> &str;
}

pub struct SimpleConsoleInterpreter<'a, C: Commands<'a>> {
    interpreter: SimpleInterpreter<'a, C>
}

impl<'a, C: Commands<'a>> SimpleConsoleInterpreter<'a, C> {
    pub fn new(commands: C) -> SimpleConsoleInterpreter<'a, C> {
        SimpleConsoleInterpreter {
            interpreter: SimpleInterpreter::new(commands),
        }
    }
    
    pub fn get_commands(&self) -> &C {
        self.interpreter.get_commands()
    }
}

impl<'a, C: Commands<'a>> ConsoleInterpreter for SimpleConsoleInterpreter<'a, C> {
    fn prompt_info(&self) -> &str {
        self.interpreter.get_commands().prompt_info()
    }
}

impl<'a, C: Commands<'a>> interpreter::Interpreter for SimpleConsoleInterpreter<'a, C> {
    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &str)
        -> bool 
    {
        self.interpreter.exec(command, args, orig)
    }
}

pub struct Console {
    interactive: bool
}

impl Console {   
    pub fn new(interactive: bool) -> Console {
        Console {
            interactive : interactive
        }
    }

    pub fn run<I: ConsoleInterpreter> (&self, interpreter: &mut I) {
        loop {
            if self.interactive {
                print!("{0}> ", interpreter.prompt_info());
                let _result = io::stdout().flush();
            }

            let mut input = String::new();
            let trimmed = match io::stdin().read_line(&mut input) {
                Err(_) => panic!("input error"),
                Ok(0) => break,
                Ok(_) => input.trim()
            };

            if trimmed == "" {
                continue;
            }

            if !interpreter.eat(&input) {
                eprintln!("unknown command: {}", input);
                
                if !self.interactive {
                    exit(-1);
                }
            }
        }
    }
}

