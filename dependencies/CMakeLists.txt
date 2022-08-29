

SET(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/libzip/lib)
SET(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/dependencies/zlib)

add_subdirectory(zlib)
add_subdirectory(libzip)

add_subdirectory(pybind11)
add_subdirectory(sqlite)
