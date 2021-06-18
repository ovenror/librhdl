add_executable(internaltests $<TARGET_OBJECTS:rhdl_obj>
	divisivewires.cpp
	fixoverlongwires.cpp
	connection.cpp
)

add_subdirectory(simulation)

target_include_directories(internaltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(internaltests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(internaltests ${GTEST_BOTH_LIBRARIES} pthread) 
target_compile_features(internaltests PRIVATE cxx_std_17)

gtest_discover_tests(internaltests)

