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
    loader.h
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
    loader.cpp
    runtime.cpp
)

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
executable(test_directory_iterator)

add_test(NAME buffer COMMAND test_buffer)
add_test(NAME pool COMMAND test_pool)

# ../dependencies/torch-cpu/libtorch/share/cmake/
SET(Torch_DIR  ../dependencies/torch-cpu/libtorch/share/cmake/Torch)

find_package(Torch)

include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Python_INCLUDE_DIRS})

IF (BUILD_PYTHON_MODULE)
    LINK_DIRECTORIES(../dependencies/torch-cpu/lib)

    ADD_LIBRARY(cpploader MODULE pyloader.cpp pytorchloader.h torchloader.cpp ${DATALOADER_SRC})
    TARGET_LINK_LIBRARIES(cpploader PUBLIC
        ${FILESYSTEM_LIB}
        ${TurboJPEG_LIBRARIES}
        ${TORCH_LIBRARIES}
        # dataloader
        pybind11::module
        # numpy stuff
        torch_python
    )
    SET_PROPERTY(TARGET cpploader PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET cpploader PROPERTY PREFIX "${PYTHON_MODULE_PREFIX}")
    SET_PROPERTY(TARGET cpploader PROPERTY SUFFIX "${PYTHON_MODULE_EXTENSION}")

    add_custom_command(TARGET cpploader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/dependencies/torch-cpu/lib"
            $<TARGET_FILE_DIR:cpploader>)
ENDIF(BUILD_PYTHON_MODULE)

ADD_EXECUTABLE(test_jit test_jit.cpp torchloader.cpp dataset.cpp ${DATALOADER_HDS} ${DATALOADER_SRC})
TARGET_LINK_LIBRARIES(test_jit
    ${TurboJPEG_LIBRARIES}
    ${TORCH_LIBRARIES}
    ${FILESYSTEM_LIB}
)
SET_PROPERTY(TARGET test_jit PROPERTY CXX_STANDARD 17)
