target_sources(rhdl_obj
	PRIVATE
		direction.cpp
		icomposite.cpp
		interface.cpp
		iplaceholder.cpp
		isingle.cpp
		predicate.cpp
)

add_subdirectory(cresult)
add_subdirectory(visitors)
