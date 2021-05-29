use std::io::Write;
use std::io::stderr;
use std::process::exit;

pub struct StdEmerg {}

impl StdEmerg {
    pub fn new() -> StdEmerg {
        StdEmerg {}
    }
}

impl Write for StdEmerg {
    fn write(&mut self, buf: &[u8]) -> std::io::Result<usize> {
        stderr().write(buf)
    }
    
    fn flush(&mut self) -> std::io::Result<()> {
        exit(-1);
    }
}

