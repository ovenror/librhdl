use std::str::Split;

pub fn split_qn(qn: &str) -> (&str, Split<char>) {
    let mut components = qn.split('.');
    let basename = components.next().unwrap().trim();

    (basename, components)
}
