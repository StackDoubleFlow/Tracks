extern crate cbindgen;

fn main() {
    {
        use std::env;
        let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

        cbindgen::Builder::new()
            .with_crate_and_name(crate_dir, "tracks_rs")
            // .with_parse_deps(true)
            .with_only_target_dependencies(true)
            .with_language(cbindgen::Language::C)
            .with_namespaces(&["Tracks", "ffi"])
            .with_cpp_compat(true)
            .with_pragma_once(true)
            .generate()
            .expect("Unable to generate bindings")
            .write_to_file("../shared/bindings.h");
    }
}
