cmake_minimum_required(VERSION 3.0.0)
project(libuv-app VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(libuv-server libuv-server.c)
add_executable(libuv-client libuv-client.c)
target_link_libraries(libuv-server uv)
target_link_libraries(libuv-client uv)

add_executable(libuv-ftp-server libuv-ftp-server.c)
add_executable(libuv-ftp-client libuv-ftp-client.c)
target_link_libraries(libuv-ftp-server uv)
target_link_libraries(libuv-ftp-client uv)

add_executable(libuv-test libuv-test.c)
target_link_libraries(libuv-test uv)

add_executable(timer-example timer-example.c)
target_link_libraries(timer-example uv)

add_executable(file-monitor file-monitor.c)
target_link_libraries(file-monitor uv)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
