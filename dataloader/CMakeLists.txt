# better way ?
INCLUDE_DIRECTORIES(.)

# Source files
# ====================================
#           (Glob is not recommended)

SET(DATALOADER_HDS
    io.h
    utils.h
    dataset.h
    ffilesystem.h
    jpeg.h
    runtime.h
    image.h
    buffer.h
    pool.h
    loader.h
    dataloader.h
    sampler.h
    zip_handles.h
)

SET(DATALOADER_SRC
    utils.cpp
    dataset.cpp
    io.cpp
    ffilesystem.cpp
    jpeg.cpp
    image.cpp
    buffer.cpp
    pool.cpp
    loader.cpp
    runtime.cpp
    sampler.cpp
    dataloader.cpp
)

find_package(TurboJPEG)
find_package(Boost COMPONENTS filesystem)

include_directories(${TurboJPEG_INCLUDE_DIRS})

set(FILESYSTEM_LIB
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    stdc++fs
    pthread
    zip
)

macro(library name)
    ADD_LIBRARY(${name} ${DATALOADER_HDS} ${DATALOADER_SRC})
    TARGET_LINK_LIBRARIES(${name} ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} zip)
    SET_PROPERTY(TARGET ${name} PROPERTY CXX_STANDARD 17)
endmacro(library)

macro(executable name)
    ADD_EXECUTABLE(${name} ${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} dataloader zip)
    SET_PROPERTY(TARGET ${name} PROPERTY CXX_STANDARD 17)
endmacro(executable)

library(dataloader)

executable(main)
executable(io_benchmark)

executable(test_buffer)
executable(test_pool)
executable(test_directory_iterator)
executable(test_dataset)
executable(test_zip)

add_test(NAME buffer COMMAND test_buffer)
add_test(NAME pool COMMAND test_pool)

# ../dependencies/torch-cpu/libtorch/share/cmake/
SET(Torch_DIR  ../dependencies/libtorch/share/cmake/Torch)

find_package(Python COMPONENTS Development)
find_package(Torch)

include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Python_INCLUDE_DIRS})

IF (BUILD_PYTHON_MODULE)
    LINK_DIRECTORIES(../dependencies/libtorch/lib)

    ADD_LIBRARY(cpploader MODULE pyloader.cpp pytorchloader.h torchloader.h torchloader.cpp)
    MESSAGE(STATUS ${TORCH_LIBRARIES})
    TARGET_LINK_LIBRARIES(cpploader PUBLIC
        ${FILESYSTEM_LIB}
        ${TurboJPEG_LIBRARIES}
        ${TORCH_LIBRARIES}

        dataloader
        pybind11::module
        # numpy stuff
        torch_python
        c10
    )
    SET_PROPERTY(TARGET cpploader PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET cpploader PROPERTY PREFIX "${PYTHON_MODULE_PREFIX}")
    SET_PROPERTY(TARGET cpploader PROPERTY SUFFIX "${PYTHON_MODULE_EXTENSION}")

    add_custom_command(TARGET cpploader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/dependencies/libtorch/lib"
            $<TARGET_FILE_DIR:cpploader>)
ENDIF(BUILD_PYTHON_MODULE)


macro(torch_executable name)
    ADD_EXECUTABLE(${name} ${name}.cpp torchloader.cpp torchloader.h dataset.cpp sampler.cpp)
    TARGET_LINK_LIBRARIES(${name} ${TORCH_LIBRARIES} ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} dataloader zip)
    SET_PROPERTY(TARGET ${name} PROPERTY CXX_STANDARD 17)
endmacro(torch_executable)

torch_executable(test_jit)

#ADD_EXECUTABLE(test_jit test_jit.cpp torchloader.cpp)
#TARGET_LINK_LIBRARIES(test_jit
#    dataloader
#    ${TurboJPEG_LIBRARIES}
#
#    ${FILESYSTEM_LIB}
#)
#SET_PROPERTY(TARGET test_jit PROPERTY CXX_STANDARD 17)
