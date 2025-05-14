extern crate bindgen;

use std::path::PathBuf;
use std::env;

fn main() {
	println!("cargo:rustc-link-search=native={}", String::from("../../lib"));
	println!("cargo:rustc-link-lib=static=rhdl");
	println!("cargo:rustc-link-lib=dylib=stdc++");

    println!("cargo:rerun-if-changed=wrapper.h");
    let mut builder = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg("-I../../include")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()));

    let items = [
        "rhdl_entity", "rhdl_iface",
        "rhdl_begin_structure",
        "rhdl_component", "rhdl_select", "rhdl_connect",
        "rhdl_finish_structure",
        "rhdl_print_commands",
        "rhdlo_get",
        "rhdlo_read_cstring", "rhdlo_has_value",
        "rhdl_errno", "rhdl_errstr",

//        "Flags::F_CREATE_STATELESS", "Flags::F_CREATE_STATEFUL",
        "Flags",
        ];

    for i in items {
        builder = builder.allowlist_item(i);
    }

    let bindings = builder
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!"); 
}

