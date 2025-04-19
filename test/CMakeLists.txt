find_package(GTest)

if (GTest_FOUND)
find_package(Threads REQUIRED)
add_subdirectory(api)
add_subdirectory(c_api)
add_subdirectory(internal)
add_subdirectory(util)
endif()
