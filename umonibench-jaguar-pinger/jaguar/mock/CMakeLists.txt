add_executable(mock_client client.cc)
target_link_libraries(mock_client ${muduo_base} ${muduo_net} pthread)

add_executable(mock_server JaguarServer.cc)
target_link_libraries(mock_server core common ${muduo_base} ${muduo_net} pthread)

add_executable(blockingqueue_bench BlockingQueue_bench.cc)
target_link_libraries(blockingqueue_bench common ${muduo_base} pthread)

add_executable(pingSender PingSender.cc)
target_link_libraries(pingSender common pthread)


add_executable(caller CallBack_test.cc)
target_link_libraries(caller ${muduo_base} ${boost} pthread)

add_executable(async_pinger PingTaskLifeCycleManager_test.cc)
target_link_libraries(async_pinger core common ${muduo_base} ${boost} pthread)