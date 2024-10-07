add_custom_target(
	rhdc
	COMMAND touch src/main.rs
	COMMAND cargo build -Z unstable-options --artifact-dir ../../bin
	DEPENDS rhdl
)
add_dependencies(rhdc rhdl)

