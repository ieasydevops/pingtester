set(ping_SRCS
		JaguarPinger.cpp
	)


message(STATUS ${DEP_PATH})
link_directories(${DEP_PATH}/lib)

include_directories(${DEP_PATH}/include)
include_directories(${RPOJECT_SOURCE_DIR}/jaguar/common)
include_directories(${RPOJECT_SOURCE_DIR}/jaguar/core)

add_executable(jaguar_pinger JaguarPinger.cpp)
target_link_libraries(jaguar_pinger core common ${muduo_base} pthread dl)
