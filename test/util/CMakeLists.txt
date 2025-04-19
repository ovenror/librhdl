add_executable(utiltests $<TARGET_OBJECTS:rhdl_obj>
	dictionary.cpp
)
target_include_directories(utiltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(utiltests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(utiltests ${GTEST_BOTH_LIBRARIES} pthread) 
target_compile_features(utiltests PRIVATE cxx_std_17)

gtest_discover_tests(utiltests)