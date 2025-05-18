use std::os::raw::c_char;
use std::{fmt, slice};
use std::ffi::CStr;
use std::iter::Map;

pub type StrIter<'a> = Map<std::slice::Iter<'a, *const i8>, fn(&*const i8) -> &'a str>;

pub struct CStrings {
    pub ptr: *const *const c_char
}

impl CStrings {
    pub fn new(ptr: *const *const c_char) -> CStrings {
        if ptr.is_null() {
            panic!("Cannot create CStrings from null pointer");
        }

        CStrings {ptr: ptr}
    }

    pub fn str_iter<'a>(&self) -> StrIter<'a>
    {
        let mut sl = unsafe {slice::from_raw_parts(self.ptr, 1)};
        let mut len = 0;

        while !sl[len].is_null() {
            len = len + 1;
            sl = unsafe {slice::from_raw_parts(self.ptr, len + 1)};
        }

        sl = unsafe {slice::from_raw_parts(self.ptr, len)};

        sl.iter().map(Self::ptr_to_str)
    }

    pub fn empty<'a>() -> StrIter<'a> {
        let sl = unsafe {slice::from_raw_parts(std::ptr::null(), 0)};
        sl.iter().map(Self::ptr_to_str)
    }

    fn ptr_to_str<'a>(ptr: &*const i8) -> &'a str {
        if ptr.is_null() {
            panic!("Tried to dereference nullptr");
        }

        unsafe{CStr::from_ptr(*ptr).to_str().unwrap()}
    }
}

impl fmt::Display for CStrings {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result  {
        for mname in self.str_iter() {
            let result = writeln!(f, "  {}", mname);

            if result.is_err() {
                return result;
            }
        }

        Ok(())
    }
}
