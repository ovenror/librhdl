extern crate bindgen;

use std::path::PathBuf;
use std::env;

fn main() {
	println!("cargo:rustc-link-search=native={}", String::from("../../lib"));
	println!("cargo:rustc-link-lib=static=rhdl");
	println!("cargo:rustc-link-lib=dylib=stdc++");

    println!("cargo:rerun-if-changed=wrapper.h");
    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg("-I../../include")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!"); 
}

