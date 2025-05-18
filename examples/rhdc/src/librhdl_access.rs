use crate::cstrings::StrIter;
use crate::librhdl::*;
use crate::cstrings::CStrings;

use std::fmt;
use std::ffi::CString;
use std::ffi::CStr;
use std::io::Write;

pub fn errstr() -> &'static str {
    let eptr = unsafe {rhdl_errstr()};
    let ecstr = unsafe {CStr::from_ptr(eptr)};

    ecstr.to_str().unwrap()
}

pub fn perror(err: &mut dyn Write) {
    let ec = unsafe {rhdl_errno()};
    let msg = errstr();
    writeln!(err, ": error #{}: {}", ec, msg).unwrap();
}

pub trait Selectable : fmt::Display {
    fn select(&self, name: &str) -> *const Self;
    fn members<'a>(&'a self) -> StrIter<'a>;
}

impl Selectable for rhdl_connector_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_connector_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdl_select(this, tname.as_ptr())}
    }
    fn members<'a>(&'a self) -> StrIter<'a> {
        unsafe{(*self.iface).members()}
    }
}

impl Selectable for rhdl_iface_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_iface_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdl_iface(this, tname.as_ptr())}
    }
    fn members<'a>(&'a self) -> StrIter<'a> {
        let u = self.__bindgen_anon_1;
        match self.type_ {
            rhdl_iface_type_RHDL_SINGLE => CStrings::empty(),
            rhdl_iface_type_RHDL_COMPOSITE => CStrings::new(unsafe{u.composite.interfaces}).str_iter(),
            rhdl_iface_type_RHDL_UNSPECIFIED => CStrings::empty(),
            _ => panic!("Unknown interface type")
        }
    }
}

impl Selectable for rhdl_object_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_object_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdlo_get(this, tname.as_ptr())}
    }
    fn members<'a>(&'a self) -> StrIter<'a> {
        CStrings::new(self.members).str_iter()
    }
}

fn translate_direction<'a>(dir: rhdl_direction) -> &'a str {
    return match dir {
        rhdl_direction_RHDL_IN => "IN",
        rhdl_direction_RHDL_OUT => "OUT",
        _ => "(error)"
    };
}

impl fmt::Display for rhdl_isingle {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let idir = self.dir;
        let dir = translate_direction(idir);
        write!(f, "Direction: {}", dir)
    }
}

impl fmt::Display for rhdl_icomposite {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", CStrings::new(self.interfaces))
    }
}

impl fmt::Display for rhdl_namespace_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if let Err(fmt::Error) = writeln!(f, "Entities:") {
            return Err(fmt::Error);
        }

        write!(f, "{}", CStrings::new(self.members))
    }
}

impl fmt::Display for rhdl_entity_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", &unsafe {*self.iface})
    }
}

impl fmt::Display for rhdl_iface_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let u = self.__bindgen_anon_1;
        match self.type_ {
            rhdl_iface_type_RHDL_SINGLE => write!(f, "{}", unsafe {u.single}),
            rhdl_iface_type_RHDL_COMPOSITE => write!(f, "{}", unsafe {u.composite}),
            rhdl_iface_type_RHDL_UNSPECIFIED => write!(f, "(unspecified)"),
            _ => write!(f, "(error)")
        }
    }
}

impl fmt::Display for rhdl_connector_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.iface.is_null() {
            panic!()
        }

        write!(f, "{}", &unsafe {*self.iface})
    }
}

impl fmt::Display for rhdl_object_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if unsafe{rhdlo_has_value(self)} != 0 {
            write!(f, "{}", unsafe{CStr::from_ptr(rhdlo_read_cstring(self))}.to_str().unwrap())
        }
        else {
            write!(f, "{}", CStrings::new(self.members))
        }
    }
}
