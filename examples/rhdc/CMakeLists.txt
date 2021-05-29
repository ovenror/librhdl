add_custom_target(
	rhdc
	COMMAND cargo build -Z unstable-options --out-dir ../../bin
	DEPENDS rhdl
)
add_dependencies(rhdc rhdl)

