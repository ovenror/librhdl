#![allow(dead_code)]

use crate::librhdl::*;
use crate::librhdl_access::*;

use std::io::Write;
use std::ptr;

struct QNAccumulator {
    qn: String
}

impl QNAccumulator {
    fn new(basename: &str) -> QNAccumulator {
        QNAccumulator {
            qn: basename.to_string()
        }
    }
}

impl QNAccumulator {
    fn add(&mut self, component: &str) {
        self.qn += ".";
        self.qn += component;
    }

    fn get_qn(&self) -> &str {
        if self.qn.starts_with(".") {
            &self.qn[1..]
        } else {
            &self.qn
        }
    }
}
trait ResolveErrorHandler {
    fn record(&mut self, _component: &str);
    fn whine(&mut self, _component: &str);
}

trait ActiveResolveErrorHandler : ResolveErrorHandler {
    fn record_(&mut self, component: &str) {
        self.accu().add(component);
    }

    fn accu(&mut self) -> &mut QNAccumulator;

    fn do_whine(base_qn: &str, component: &str, stream: &mut dyn Write)
    {
        write!(stream, "{} contains no member named \"{}\"", base_qn, component).unwrap();
        perror(stream)
    }
}

struct NOPResolveErrorHandler {}

impl NOPResolveErrorHandler {
    pub fn new() -> NOPResolveErrorHandler
    {
        NOPResolveErrorHandler {}
    }
}

impl ResolveErrorHandler for NOPResolveErrorHandler {
    fn record(&mut self, _component: &str) {}
    fn whine(&mut self, _component: &str) {}
}

struct PrintingResolveErrorHandler<'a> {
    stream: &'a mut dyn Write,
    accu: QNAccumulator
}

impl<'a> PrintingResolveErrorHandler<'a> {
    pub fn new(stream: &'a mut dyn Write, basename: &str) -> PrintingResolveErrorHandler<'a>
    {
        PrintingResolveErrorHandler {
            stream: stream,
            accu: QNAccumulator::new(basename)
        }
    }
}

impl<'a> ResolveErrorHandler for PrintingResolveErrorHandler<'a> {
    fn record(&mut self, component: &str) {self.record_(component);}
    fn whine(&mut self, component: &str )
    {
        Self::do_whine(self.accu.get_qn(), component, self.stream);
    }
}

impl<'a> ActiveResolveErrorHandler for PrintingResolveErrorHandler<'a> {
    fn accu(&mut self) -> &mut QNAccumulator {&mut self.accu}
}

struct DeferringResolveErrorHandler {
    accu: QNAccumulator,
    last_component: String
}

impl<'a> DeferringResolveErrorHandler {
    pub fn new(basename: &'a str) -> DeferringResolveErrorHandler {
        DeferringResolveErrorHandler {
            accu: QNAccumulator::new(basename),
            last_component: "".to_string()
        }
    }

    pub fn whine_later(&self, stream: &mut dyn Write)
    {
        Self::do_whine(self.accu.get_qn(), &self.last_component, stream);
    }
}

impl ResolveErrorHandler for DeferringResolveErrorHandler {
    fn record(&mut self, component: &str) {self.record_(component);}
    fn whine(&mut self, component: &str) {self.last_component = component.to_string()}
}

impl ActiveResolveErrorHandler for DeferringResolveErrorHandler {
    fn accu(&mut self) -> &mut QNAccumulator {&mut self.accu}
}

trait ResolveErrorCollector<H: ResolveErrorHandler> {
    fn new_handler(&mut self, basename: &str) -> &mut H;
    fn whine(&mut self);
}

struct PrintingResolveErrorCollector<'a> {
    stream: &'a mut dyn Write,
    handlers: Vec<DeferringResolveErrorHandler>
}

impl<'a> PrintingResolveErrorCollector<'a> {
    fn new(stream: &'a mut dyn Write) -> Self
    {
        Self {stream, handlers: Vec::new()}
    }
}

impl<'a> ResolveErrorCollector<DeferringResolveErrorHandler>
        for PrintingResolveErrorCollector<'a>
{
    fn new_handler(&mut self, basename: &str) -> &mut DeferringResolveErrorHandler {
        self.handlers.push(DeferringResolveErrorHandler::new(basename));
        self.handlers.last_mut().unwrap()
    }

    fn whine(&mut self) {
        for handler in self.handlers.iter() {
            handler.whine_later(&mut self.stream);
        }
    }
}

struct NOPResolveErrorCollector {
    handler: NOPResolveErrorHandler
}

impl NOPResolveErrorCollector {
    fn new() -> Self
    {
        Self {handler: NOPResolveErrorHandler::new()}
    }
}

impl ResolveErrorCollector<NOPResolveErrorHandler>
        for NOPResolveErrorCollector
{
    fn new_handler(&mut self, _basename: &str) -> &mut NOPResolveErrorHandler {
        &mut self.handler
    }

    fn whine(&mut self) {}
}

pub type QNSlice<'a> = [&'a str];

pub fn resolve_object_noerr(qn : &QNSlice) -> *const rhdl_object_t
{
    resolve_object(qn, &mut NOPResolveErrorHandler::new())
}

pub fn resolve_object_err(qn : &QNSlice, err: &mut dyn Write) -> *const rhdl_object_t
{
    resolve_object(qn, &mut PrintingResolveErrorHandler::new(err, ""))
}

fn resolve_object<E: ResolveErrorHandler>(qn : &QNSlice, err: &mut E) -> *const rhdl_object_t
{
    resolve_with_base(rhdl_object_t::root(), qn, err)
}

pub fn resolve_namespace_noerr(qn : &QNSlice) -> *const rhdl_namespace_t
{
    resolve_namespace(qn, &mut NOPResolveErrorHandler::new())
}

pub fn resolve_namespace_err(qn : &QNSlice, err: &mut dyn Write) -> *const rhdl_namespace_t
{
    resolve_namespace(qn, &mut PrintingResolveErrorHandler::new(err, ""))
}

fn resolve_namespace<E: ResolveErrorHandler>(qn : &QNSlice, err: &mut E) -> *const rhdl_namespace_t
{
    resolve_with_base(rhdl_namespace_t::root(), qn, err)
}

pub fn resolve_with_object_noerr(base: *const rhdl_object_t, qn : &QNSlice) -> *const rhdl_object_t
{
    resolve_with_base(base, qn, &mut NOPResolveErrorHandler::new())
}

pub fn resolve_with_base_noerr<S: Selectable>(base : *const S, qn : &QNSlice) -> *const S
{
    resolve_with_base(base, qn, &mut NOPResolveErrorHandler::new())
}

pub fn resolve_with_base_err<S: Selectable>(base : *const S, qn : &QNSlice, basename: &str, err: &mut dyn Write) -> *const S
{
    resolve_with_base(base, qn, &mut PrintingResolveErrorHandler::new(err, basename))
}

fn resolve_with_nullable_base<S: Root, E: ResolveErrorHandler>(base : *const S, qn : &QNSlice) -> *const S
{
    let guaranteed_base = if base.is_null() {
        S::root()
    } else {
        base
    };

    resolve_with_base(guaranteed_base, qn, &mut NOPResolveErrorHandler::new())
}

fn resolve_with_base<S: Selectable, E: ResolveErrorHandler>(base : *const S, qn : &QNSlice, err: &mut E) -> *const S
{
    assert!(!base.is_null());

    if qn.len() == 0 {
        return base
    }

    assert!(qn[0] != "");

    let mut curbase = base;

    for component in qn {
        assert!(!component.contains(char::is_whitespace));

        curbase = unsafe{(*curbase).select(component)};

        if curbase.is_null() {
            err.whine(component);
            return ptr::null()
        }

        err.record(component)
    }

    return curbase
}

pub fn resolve_with_bases_noerr<S: Selectable, R: ResolveResult<S>>(bases : &Vec<(*const S, &str)>, qn : &QNSlice) -> R
{
    resolve_with_bases(bases, qn, &mut NOPResolveErrorCollector::new())
}

pub fn resolve_with_bases_err<S: Selectable, R: ResolveResult<S>>(bases : &Vec<(*const S, &str)>, qn : &QNSlice, err: &mut dyn Write) -> R
{
    resolve_with_bases(bases, qn, &mut PrintingResolveErrorCollector::new(err))
}

fn resolve_with_bases<
        S: Selectable, E: ResolveErrorHandler, C: ResolveErrorCollector<E>,
        R: ResolveResult<S>> (
                bases : &Vec<(*const S, &str)>, qn : &QNSlice, err: &mut C) -> R
{
    let mut result = R::new();

    for (base, basename) in bases.into_iter() {
        let handler = err.new_handler(basename);
        let resolved = resolve_with_base(*base, qn, handler);

        if !resolved.is_null() {
            result.add(resolved);
        }

        if result.done() {
            break
        }
    }

    if result.is_empty() {
        err.whine()
    }

    result
}

pub trait ResolveResult<S: Selectable> {
    fn new() -> Self;
    fn add(&mut self, resolved: *const S);
    fn done(&self) -> bool;
    fn is_empty(&self) -> bool;
}


impl<S: Selectable> ResolveResult<S> for *const S {
    fn new() -> Self {
        ptr::null()
    }

    fn add(&mut self, resolved: *const S) {
        *self = resolved;
    }

    fn done(&self) -> bool {
        !self.is_empty()
    }

    fn is_empty(&self) -> bool {
        self.is_null()
    }
}

impl<S: Selectable> ResolveResult<S> for Vec<*const S> {
    fn new() -> Self {
        Vec::new()
    }

    fn add(&mut self, resolved: *const S) {
        self.push(resolved)
    }

    fn done(&self) -> bool {
        false
    }

    fn is_empty(&self) -> bool {
        self.is_empty()
    }
}


#[cfg(test)]
mod tests {
    use std::ffi::{CStr, CString};
    use super::*;

    #[test]
    fn failed_resolve() {
        assert!(resolve_object_noerr(&["doesnotexist"]) == ptr::null());
        assert!(unsafe{rhdl_errno()} == Errorcode_E_NO_SUCH_MEMBER);
    }

    #[test]
    fn failed_complex_resolve() {
        assert!(resolve_object_noerr(&["entities", "Inverter", "interface", "bernd"]) == ptr::null());
        assert!(unsafe{rhdl_errno()} == Errorcode_E_NO_SUCH_MEMBER);
    }

    #[test]
    fn root_resolve() {
        let root = resolve_object_noerr(&[]);
        assert!(!root.is_null());
        assert!(unsafe{CStr::from_ptr((*root).name)}.to_str().unwrap() == "root");
    }

    #[test]
    fn toplevel_resolve() {
        assert!(resolve_object_noerr(&["entities"]) != ptr::null());
    }

    #[test]
    fn secondlevel_resolve() {
        assert!(resolve_object_noerr(&["entities", "Inverter"]) != ptr::null());
    }

    #[test]
    fn complex_resolve() {
        assert!(resolve_object_noerr(&["entities", "AND", "representations", "AND_Structure_0", "content"]) != ptr::null());
    }

    #[test]
    fn based_resolve() {
        let entities = resolve_object_noerr(&["entities"]);
        assert!(!entities.is_null());
        assert!(!resolve_with_object_noerr(entities, &["ClockDiv2"]).is_null());
    }

    #[test]
    fn typed_based_resolve() {
        let name = CString::new("NAND").unwrap();
        let entity = unsafe{rhdl_entity(ptr::null(), name.as_ptr())};
        assert!(!resolve_with_base_noerr(unsafe{*entity}.iface, &["in", "bit1"]).is_null());
    }

    #[test]
    fn resolve_with_1_bases() {
        let entities_cstr = CString::new("entities").unwrap();
        let entities = unsafe{rhdlo_get(ptr::null(), entities_cstr.as_ptr())};
        let result: Vec<*const rhdl_object_t> = resolve_with_bases_noerr(
            &vec![(entities, "entities")],
            &["Inverter", "interface", "in"]);
        assert!(result.len() == 1);
        assert!(!result.last().unwrap().is_null())
    }

    #[test]
    fn resolve_with_3_bases() {
        let root = unsafe{rhdlo_get(ptr::null(), ptr::null())};
        let entities_cstr = CString::new("entities").unwrap();
        let entities = unsafe{rhdlo_get(root, entities_cstr.as_ptr())};
        let trans_cstr = CString::new("transformations").unwrap();
        let trans = unsafe{rhdlo_get(root, trans_cstr.as_ptr())};
        let result: Vec<*const rhdl_object_t> = resolve_with_bases_noerr(
            &vec![(root, ""), (entities, "entities"), (trans, "transformations")],
            &["Inverter", "interface", "in"]);
        assert!(result.len() == 1);
        assert!(!result.last().unwrap().is_null())
    }

    #[test]
    fn failed_resolve_with_3_bases() {
        let root = unsafe{rhdlo_get(ptr::null(), ptr::null())};
        let entities_cstr = CString::new("entities").unwrap();
        let entities = unsafe{rhdlo_get(root, entities_cstr.as_ptr())};
        let trans_cstr = CString::new("transformations").unwrap();
        let trans = unsafe{rhdlo_get(root, trans_cstr.as_ptr())};
        let result: Vec<*const rhdl_object_t> = resolve_with_bases_noerr(
            &vec![(root, ""), (entities, "entities"), (trans, "transformations")],
            &["Inverter", "lol", "bla"]);
        assert!(result.is_empty());
    }

    #[test]
    fn resolve_namespace() {
        assert!(!resolve_namespace_noerr(&["entities"]).is_null());
    }

    #[test]
    fn resolve_nonexisting_namespace() {
        assert!(resolve_namespace_noerr(&["entities", "olololol"]).is_null());
        assert!(unsafe{rhdl_errno()} == Errorcode_E_NO_SUCH_MEMBER);
    }

}