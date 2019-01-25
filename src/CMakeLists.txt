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

add_test(NAME buffer COMMAND test_buffer)
add_test(NAME pool COMMAND test_pool)


SET(PYDATALOADER_HDS
    torchloader.h
)

SET(PYDATALOADER_SRC
    torchloader.cpp
)


SET(Torch_DIR  ../dependencies/torch-cpu/share/cmake/Torch)

find_package(Python)
find_package(Torch)

#MESSAGE(STATUS ${TORCH_LIBRARY} ${TORCH_INCLUDE_DIRS})
include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Python_INCLUDE_DIRS})

ADD_LIBRARY(cpploader MODULE pyloader.cpp ${PYDATALOADER_HDS} ${PYDATALOADER_SRC} ${DATALOADER_SRC})
TARGET_LINK_LIBRARIES(cpploader PUBLIC
    ${FILESYSTEM_LIB}
    ${TurboJPEG_LIBRARIES}
    ${TORCH_LIBRARIES}
    # dataloader
    pybind11::module
)
SET_PROPERTY(TARGET cpploader
    PROPERTY CXX_STANDARD 17
    #PROPERTY PREFIX "${PYTHON_MODULE_PREFIX}"
    #PROPERTY SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

#pybind11_add_module(pyloader pyloader.cpp)
#TARGET_LINK_LIBRARIES(pyloader ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} ${TORCH_LIBRARIES})

# why...
SET(MISSING_SYMBOLS
    loader.cpp
    dataset.cpp
    ffilesystem.cpp
)

#ADD_EXECUTABLE(test_pyloader test_pyloader.cpp
#    ${PYDATALOADER_HDS}
#    ${PYDATALOADER_SRC}
#    ${MISSING_SYMBOLS}
#)
#TARGET_LINK_LIBRARIES(test_pyloader
#    ${FILESYSTEM_LIB}
#    ${TurboJPEG_LIBRARIES}
#    ${TORCH_LIBRARIES}
#    dataloader
#)
#SET_PROPERTY(TARGET test_pyloader PROPERTY CXX_STANDARD 17)

#add_test(NAME pyloader COMMAND test_pyloader)
