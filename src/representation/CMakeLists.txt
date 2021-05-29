target_sources(rhdl_obj
	PRIVATE
		representation.cpp
		representationbase.cpp
		representations.cpp
)

add_subdirectory(behavioral)
add_subdirectory(blocks)
add_subdirectory(netlist)
add_subdirectory(structural)
add_subdirectory(txt)
