add_custom_target(
	rhdc
	COMMAND touch src/main.rs
	COMMAND cargo build -Z unstable-options --out-dir ../../bin
	DEPENDS rhdl
)
add_dependencies(rhdc rhdl)

