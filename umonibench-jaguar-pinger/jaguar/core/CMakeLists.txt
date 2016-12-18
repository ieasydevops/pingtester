cmake_minimum_required(VERSION 2.6)
set(core_FILES
        PingCore.h
        PingCore.cpp
        JaguarPingerServer.h
        JaguarPingerServer.cpp
        PingExecutor.h
        PingExecutor.cpp
        PingReceiver.h
        PingReceiver.cpp
        PingTaskGen.h
        PingTaskGen.cpp
        PingChecker.h
        PingChecker.cpp
        PingReporter.h
        PingReporter.cpp
        )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${RPOJECT_SOURCE_DIR}/lib)

include_directories(${DEP_PATH}/include)
add_library(core STATIC ${core_FILES})
target_link_libraries(core common ${muduo_base} ${muduo_net} pthread dl)

