target_sources(rhdl_obj
	PRIVATE
		blockssim.cpp
		exposableinternalstatesim.cpp
		hierarchicalsim.cpp
		invertersim.cpp
		netlistsim.cpp
		simulator.cpp
)

add_subdirectory(blocks)
add_subdirectory(fast)
