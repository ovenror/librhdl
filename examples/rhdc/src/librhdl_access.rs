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

/*
    fn fmt_member(&self, member: &str, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
        let mptr = self.select(member)
    }
*/

    fn default_fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        for m in self.members() {
            match write!(f, "  {} : ", m) {
                Ok(()) => (),
                r => return r
            }

            let mp = self.select(m);

            if mp.is_null() {
                panic!("WTF, why does >>>{}<<< not exist?!: {}", m, unsafe{CStr::from_ptr(rhdl_errstr())}.to_str().unwrap() );
            }

            match unsafe{(*mp).fmt_short(f)} {
                Ok(()) => (),
                r => return r
            }

            match writeln!(f, "") {
                Ok(()) => (),
                r => return r
            }
        }

        Ok(())
    }

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
        self.fmt(f)
    }
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

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
        unsafe{(*self.iface).fmt_short(f)}
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

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let u = self.__bindgen_anon_1;
        match self.type_ {
            rhdl_iface_type_RHDL_SINGLE => unsafe{u.single}.fmt_short(f),
            rhdl_iface_type_RHDL_COMPOSITE => unsafe{u.composite}.fmt_short(f),
            rhdl_iface_type_RHDL_UNSPECIFIED => write!(f, "(unspecified)"),
            _ => write!(f, "(error)")
        }
    }

}

impl rhdl_icomposite {
    fn members<'a>(&'a self) -> StrIter<'a> {
        CStrings::new(self.interfaces).str_iter()
    }

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Composite ({} members)", self.members().len())
    }
}

impl rhdl_isingle {
    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Single ({})", self)
    }
}

impl Selectable for rhdl_namespace_t {
    fn select(&self, name: &str) -> *const Self {
        let this: *const rhdl_namespace_t = self;
        let tname = CString::new(name).unwrap();
        unsafe {rhdl_namespace(this, tname.as_ptr())}
    }

    fn members<'a>(&'a self) -> StrIter<'a> {
        CStrings::new(self.members).str_iter()
    }

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
        write!(f, "Namespace")
    }
}

fn translate_otype<'a>(t: rhdl_type) -> &'a str {
    unsafe{CStr::from_ptr(rhdl_type_names[t as usize])}.to_str().unwrap()
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

    fn fmt_short(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
        match write!(f, "{}", translate_otype(self.type_)) {
            Ok(()) => (),
            r => return r
        }

        if unsafe{rhdlo_has_value(self)} != 0 {
            write!(f, " = {}", unsafe{CStr::from_ptr(rhdlo_read_cstring(self))}.to_str().unwrap())
        } else {
            Ok(())
        }
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

impl fmt::Display for rhdl_namespace_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        self.default_fmt(f)
    }
}

impl fmt::Display for rhdl_entity_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        unsafe{*self.iface}.fmt(f)
    }
}

impl fmt::Display for rhdl_iface_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let u = self.__bindgen_anon_1;
        match self.type_ {
            rhdl_iface_type_RHDL_SINGLE => unsafe{u.single}.fmt(f),
            rhdl_iface_type_RHDL_COMPOSITE => self.default_fmt(f),
            rhdl_iface_type_RHDL_UNSPECIFIED => write!(f, "(unspecified)"),
            _ => write!(f, "(error)")
        }
    }
}

impl fmt::Display for rhdl_connector_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if unsafe{*self.iface}.type_ == rhdl_iface_type_RHDL_COMPOSITE {
            /* Because the structure during structure definition does not
             * have a complete working interface */
            self.default_fmt(f)
        } else {
            unsafe{*self.iface}.fmt(f)
        }
    }
}

impl fmt::Display for rhdl_object_t {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if unsafe{rhdlo_has_value(self)} != 0 {
            write!(f, "{}", unsafe{CStr::from_ptr(rhdlo_read_cstring(self))}.to_str().unwrap())
        }
        else {
            self.default_fmt(f)
        }
    }
}
