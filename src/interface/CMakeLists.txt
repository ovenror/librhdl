target_sources(rhdl_obj
	PRIVATE
		direction.cpp
		compositedirection.cpp
		icomposite.cpp
		interface.cpp
		isingle.cpp
		predicate.cpp
)

add_subdirectory(cresult)
add_subdirectory(visitors)
