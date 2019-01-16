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
)

SET(DATALOADER_SRC
    utils.cpp
    dataloader.cpp
    dataset.cpp
    io.cpp
    ffilesystem.cpp
    jpeg.cpp
    image.cpp
)

#find_package(Torch)
find_package(TurboJPEG)
find_package(Boost COMPONENTS filesystem)

include_directories(${TurboJPEG_INCLUDE_DIRS})


set(FILESYSTEM_LIB
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    stdc++fs
)



# main library (prevent recompilation when building tests)
ADD_LIBRARY(dataloader ${DATALOADER_HDS} ${DATALOADER_SRC})
TARGET_LINK_LIBRARIES(dataloader ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES})
SET_PROPERTY(TARGET dataloader PROPERTY CXX_STANDARD 17)


#  main executable
# ==========================
#set(CMAKE_CXX_STANDARD 17)
ADD_EXECUTABLE(main main.cpp)
TARGET_LINK_LIBRARIES(main stdc++fs ${TurboJPEG_LIBRARIES} dataloader)
SET_PROPERTY(TARGET main PROPERTY CXX_STANDARD 17)

ADD_EXECUTABLE(io_benchmark io_benchmark.cpp)
TARGET_LINK_LIBRARIES(io_benchmark ${FILESYSTEM_LIB} ${TurboJPEG_LIBRARIES} dataloader)
SET_PROPERTY(TARGET io_benchmark PROPERTY CXX_STANDARD 17)

 


