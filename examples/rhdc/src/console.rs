use std::str::SplitWhitespace;
use std::io;
use std::io::Write;
use std::process::exit;

use crate::interpreter;
use crate::interpreter::Interpreter;
use crate::interpreter::SimpleInterpreter;
use crate::wod::WriteOrDie;
use crate::stdemerg::StdEmerg;

use rustyline_derive::{Helper, Completer, Highlighter, Hinter, Validator};
use rustyline::error::ReadlineError;
use rustyline::history::DefaultHistory;
use rustyline::Editor;
use rustyline::hint::HistoryHinter;
use rustyline::completion::{Completer, Pair};
use rustyline::Context;

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

/*
pub trait AutoCommands<'a> : Commands<'a> {
    fn auto_new(outputs: Outputs) -> Self; 
}
*/

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
    fn eat(self : &mut Self, line: &String) -> bool
    {
        self.interpreter.eat(line)
    }

    fn exec(self : &mut Self, command: &str, args: &mut SplitWhitespace, orig: &String) -> bool
    {
        self.interpreter.exec(command, args, orig)
    }
}

impl<'a, C: Commands<'a>> Completer for SimpleConsoleInterpreter<'a, C> {
    type Candidate = Pair;

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        self.interpreter.complete(line, pos, ctx)
    }
}

pub struct Console {
    interactive: bool
}

/*
impl<'a> Completer for &'a mut Interpreter<Candidate = Pair> {
    type Candidate = Pair;
}
*/

#[derive(Helper, Completer, Highlighter, Hinter, Validator)]
struct MyHelper<I: Interpreter> {
//struct MyHelper<'a, C: Completer> {
    #[rustyline(Completer)]
    completer: I,
    //#[rustyline(Highlighter)]
    //highlighter: MatchingBracketHighlighter,
    //#[rustyline(Validator)]
    //validator: MatchingBracketValidator,
    #[rustyline(Hinter)]
    hinter: HistoryHinter,
    //colored_prompt: String,
}
    
//impl Console where for <'a'> &'a mut I: Completer {   
impl Console {   
    pub fn new(interactive: bool) -> Console {
        Console {
            interactive : interactive
        }
    }

    pub fn run<I: ConsoleInterpreter> (&self, interpreter: I)
    {
        // `()` can be used when no completer is required
        let mut rl = Editor::<MyHelper::<I>, DefaultHistory>::new().unwrap();
        let h = MyHelper::<I> {
            completer: interpreter,
            //highlighter: MatchingBracketHighlighter::new(),
            hinter: HistoryHinter::new(),
            //colored_prompt: "".to_owned(),
            //validator: MatchingBracketValidator::new()
        };
        rl.set_helper(Some(h));

        /*
        if rl.load_history("history.txt").is_err() {
            println!("No previous history.");
        }*/

        loop {
            if self.interactive {
                print!("{0}> ",
                    rl.helper_mut().unwrap().completer.prompt_info());

                let _result = io::stdout().flush();
            }

            let input : String;

            let prompt = format!("{0}> ", 
                rl.helper_mut().unwrap().completer.prompt_info());

            let readline = rl.readline(&prompt);
            
            match readline {
                Ok(line) => {
                    rl.add_history_entry(line.as_str()).unwrap();
                    input = line
                },
                Err(ReadlineError::Interrupted) => {
                    println!("CTRL-C");
                    break
                },
                Err(ReadlineError::Eof) => {
                    println!("CTRL-D");
                    break
                },
                Err(err) => {
                    panic!("input error: {}", err);
                }
            }

            let trimmed = input.trim(); 

            if trimmed == "" {
                continue;
            }

            if !rl.helper_mut().unwrap().completer.eat(&input) {
                eprintln!("unknown command: {}", input);
                
                if !self.interactive {
                    exit(-1);
                }
            }
        }

        _ = rl.save_history("history.txt");
    }
}

