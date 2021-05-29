target_sources(rhdl_obj
	PRIVATE
		blocks2commands.cpp
		structure2netlist.cpp
		transformation.cpp
		transformations.cpp
		transformer.cpp
		typedtransformation.cpp
)

add_subdirectory(bgtree)
