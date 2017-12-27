include_directories("${PROJECT_SOURCE_DIR}/include")

# zlib support
if(ENABLE_ZLIB)
	include_directories(${ZLIB_INCLUDE_DIRS})
	link_libraries(${ZLIB_LIBRARIES})
	add_definitions(-DENABLE_ZLIB)
endif()

# Library
set(mosaic_src attr.c image.c io.c)
add_library(mosaic SHARED ${mosaic_src})

# Moscat utility
add_executable(moscat moscat.c)
target_link_libraries(moscat mosaic)

install(TARGETS mosaic moscat
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin)
