cmake_minimum_required(VERSION 2.8)
set(PROJECT_VERSION 1.3.0)
if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(mosaic VERSION ${PROJECT_VERSION})
else()
	project(mosaic)
endif()

find_package(ZLIB)
if(ZLIB_FOUND)
	option(ENABLE_ZLIB "Enable zlib attribute compression" ON)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Build stuff
add_subdirectory(src)
add_subdirectory(pkg-config)

# Test
# option(ENABLE_TESTS "Enable tests to be built and run with `make test`" OFF)
# if(ENABLE_TESTS)
	# enable_testing()
	# add_subdirectory(test)
# endif()

# Examples
# option(COMPILE_EXAMPLES "Compile the examples" OFF)
# if(COMPILE_EXAMPLES)
	# add_subdirectory(examples)
# endif()

# Install headers
file(GLOB headers "include/mosaic/*.h")
configure_file("include/mosaic.h" "mosaic.h" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/mosaic.h" DESTINATION "include")
install(FILES ${headers} DESTINATION "include/mosaic")
