# better way ?
INCLUDE_DIRECTORIES(.)

# Source files
# ====================================
#           (Glob is not recommended)

SET(DATALOADER_HDS
    io.h
    utils.h
    dataloader.h
    dataset.h
    ffilesystem.h
    jpeg.h
    runtime.h
    image.h
    buffer.h
    pool.h
)

SET(DATALOADER_SRC
    utils.cpp
    dataloader.cpp
    dataset.cpp
    io.cpp
    ffilesystem.cpp
    jpeg.cpp
    image.cpp
    buffer.cpp
    pool.cpp
)

#find_package(Torch)
find_package(TurboJPEG)
find_package(Boost COMPONENTS filesystem)

include_directories(${TurboJPEG_INCLUDE_DIRS})

set(FILESYSTEM_LIB
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    stdc++fs
    pthread
)

macro(library name)
    ADD_LIBRARY(${name} ${DATALOADER_HDS} ${DATALOADER_SRC})
    TARGET_LINK_LIBRARIES(${name} ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES})
    SET_PROPERTY(TARGET ${name} PROPERTY CXX_STANDARD 17)
endmacro(library)

macro(executable name)
    ADD_EXECUTABLE(${name} ${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} dataloader)
    SET_PROPERTY(TARGET ${name} PROPERTY CXX_STANDARD 17)
endmacro(executable)

library(dataloader)

executable(main)
executable(io_benchmark)

executable(test_buffer)
executable(test_pool)

add_test(NAME buffer COMMAND test_buffer)
add_test(NAME pool COMMAND test_pool)

