cmake_minimum_required(VERSION 3.5)

project(socket_message_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET socket_message_server)

file(GLOB src
	${CMAKE_SOURCE_DIR}/*.cpp
)

add_executable(${TARGET}
	${src}
)

# 配置链接参数
target_link_libraries(
        ${TARGET}
        pthread
        mysqlclient
)


