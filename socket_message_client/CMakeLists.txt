cmake_minimum_required(VERSION 3.5)

project(socket_message_client)

set(POSITION_INDEPENDENT_CODE True)

# 配置动态库生成路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
# 配置可执行程序的生成路径。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(socket_message_sdk)
add_subdirectory(socket_message_client)
