cmake_minimum_required(VERSION 2.6)

set(common_SRCS Singleton.h
        PingEntry.h
        PingEntry.cpp
        LengthHeaderCodec.h
        IcmpPkt.h
        IcmpPkt.cpp
        IcmpSockets.h
        IcmpSockets.cpp
        PingQue.h
        Context.h
        Context.cpp
        Msg.h
        Msg.cpp
        PingerMsgCodec.h
        PingerMsgCodec.cpp
        PingTaskLifeCycleManager.h
        PingTaskLifeCycleManager.cpp
        )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
message(STATUS "${PROJECT_SOURCE_DIR}")
set(CMAKE_INCLUDE_PATH ${DEP_PATH}/include)
link_directories(${DEP_PATH}/lib)
include_directories(${DEP_PATH}/include)
ADD_LIBRARY(common STATIC ${common_SRCS})
target_link_libraries(common ${muduo_base} ${muduo_net} pthread)
