use std::os::raw::c_char;
use std::{slice,fmt};
use std::ffi::CStr;

pub struct CStrings {
    pub ptr: *const *const c_char
}

impl CStrings {
    pub fn new(ptr: *const *const c_char) -> CStrings {
        CStrings {ptr: ptr}
    }
}

impl fmt::Display for CStrings {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result  {
        if self.ptr.is_null() {
            panic!();
        }

        let mut sl = unsafe {slice::from_raw_parts(self.ptr, 1)};
        let mut len = 0;

        while !sl[len].is_null() {
            len = len + 1;
            sl = unsafe {slice::from_raw_parts(self.ptr, len + 1)};
        }

        sl = unsafe {slice::from_raw_parts(self.ptr, len)};

        for ename in sl {
            let result = match unsafe {CStr::from_ptr(*ename)}.to_str() {
                Ok(name) => writeln!(f, "  {}", name),
                Err(_) => return Err(fmt::Error)
            };

            if let Err(fmt::Error) = result {
                return Err(fmt::Error);
            }
        }

        Ok(())
    }
}
