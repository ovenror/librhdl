add_executable(c_apitests
	construction.c construction.cpp
	misconstruction.c misconstruction.cpp
)

add_executable(lstest
	ls.c ls.cpp
)

target_include_directories(c_apitests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(lstest PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(c_apitests rhdl ${GTEST_BOTH_LIBRARIES} pthread) 
target_link_libraries(lstest rhdl ${GTEST_BOTH_LIBRARIES} pthread)

gtest_discover_tests(c_apitests)
gtest_discover_tests(lstest)
