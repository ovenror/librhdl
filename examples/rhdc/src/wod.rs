use std::io::Write;
use std::process::exit;

pub struct WriteOrDie {
   w : Box<dyn Write> 
}

impl WriteOrDie {
    pub fn new(w : Box<dyn Write>) -> WriteOrDie {
        WriteOrDie {w: w}
    }
}

impl Write for WriteOrDie {
    fn write(&mut self, buf: &[u8]) -> std::io::Result<usize> {
        let result = self.w.write(buf);

        if result.is_err() {
            exit(-1);
        }

        return result;
    }
    
    fn flush(&mut self) -> std::io::Result<()> {
        return self.w.flush();
    }
}
