cmake_minimum_required(VERSION 3.5)

project(socket_message_client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET socket_message_client)

file(GLOB_RECURSE src
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${TARGET}
	${src}
)

target_include_directories(
	${TARGET} PRIVATE
	${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
	${TARGET}
	socket_message_sdk
)
