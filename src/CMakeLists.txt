# better way ?
INCLUDE_DIRECTORIES(.)

# Source files
# ====================================
#           (Glob is not recommended)

SET(DATALOADER_HDS
    io.h
    jpeg_loader.h
    utils.h
    dataloader.h
    dataset.h
)

SET(DATALOADER_SRC
    jpeg_loader.cpp
    utils.cpp
    dataloader.cpp
    dataset.cpp
    io.cpp
)

find_package(TurboJPEG)
include_directories(${TurboJPEG_INCLUDE_DIRS})

# main library (prevent recompilation when building tests)
ADD_LIBRARY(dataloader ${DATALOADER_HDS} ${DATALOADER_SRC})
TARGET_LINK_LIBRARIES(dataloader stdc++fs ${TurboJPEG_LIBRARIES})
SET_PROPERTY(TARGET dataloader PROPERTY CXX_STANDARD 17)


#  main executable
# ==========================
#set(CMAKE_CXX_STANDARD 17)
ADD_EXECUTABLE(main main.cpp)
TARGET_LINK_LIBRARIES(main stdc++fs ${TurboJPEG_LIBRARIES} dataloader)
SET_PROPERTY(TARGET main PROPERTY CXX_STANDARD 17)

ADD_EXECUTABLE(io_benchmark io_benchmark.cpp)
TARGET_LINK_LIBRARIES(io_benchmark stdc++fs ${TurboJPEG_LIBRARIES} dataloader)
SET_PROPERTY(TARGET io_benchmark PROPERTY CXX_STANDARD 17)




