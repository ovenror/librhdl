use rustyline::completion::Completer;

pub struct RHDCCompleter {}

impl Completer for RHDCCompleter {
    type Candidate = Pair;

    fn get(&mut self, basename : &str, components: &mut Split<char>)
            -> rhdl_object
    {
        let ns;

        if basename == "" {
            ns = unsafe {rhdl_get(ptr::null(), ptr::null())};
        } else {
            ns = unsafe {
                rhdl_get(ptr::null(), CString::new(basename).unwrap())
            }
        }

        return unsafe{&*ns};
    }

    fn complete(&self, line: &str, pos: usize, ctx: &Context<'_>)
            -> Result<(usize, Vec<Self::Candidate>), ReadlineError>
    {
        let (basename, mut components) = split_qn(&line);
        
        let mut curbase = basename;
        let mut accu = basename;

        for component in components {
            let o = self.get(curbase, component);

            if o.is_null() {
                break;
            }

            accu = "{}.{}".format(accu, component);
            curbase = component;
        }

        let mut vec = Vec::<Pair>::new();
        vec.push(Pair::new(accu, accu));
        return Ok((0, vec))
    }
}

