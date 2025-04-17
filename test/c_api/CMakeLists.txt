add_executable(c_apitests
	construction.c construction.cpp utils.c
	misconstruction.c misconstruction.cpp
	namespaces.c namespaces.cpp
)

add_executable(lstest
	ls.c ls.cpp utils.c
)

add_executable(objectstest
	objects.c objects.cpp utils.c
)

target_include_directories(c_apitests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(lstest PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(objectstest PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(c_apitests rhdl ${GTEST_BOTH_LIBRARIES} pthread) 
target_link_libraries(lstest rhdl ${GTEST_BOTH_LIBRARIES} pthread)
target_link_libraries(objectstest rhdl ${GTEST_BOTH_LIBRARIES} pthread)

gtest_discover_tests(c_apitests)
gtest_discover_tests(lstest)
gtest_discover_tests(objectstest)
