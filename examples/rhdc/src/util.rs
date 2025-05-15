pub fn split_qn_once(qn: &str) -> (&str, &str)
{
    match qn.split_once('.') {
        Some((a,b)) => (a,b),
        None => (qn, "")
    }
}
