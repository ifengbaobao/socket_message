cmake_minimum_required(VERSION 3.5)

set(TARGET socket_message_sdk)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(${TARGET} SHARED ${src})

target_include_directories(
	${TARGET} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PUBLIC Threads::Threads)