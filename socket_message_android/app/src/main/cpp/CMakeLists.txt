# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

set(TARGET socket_message_android)

# Declares and names the project.

project("socket_message_android")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             ${TARGET}

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             com_ifengbaobao_socket_message_android_SocketMessageAndroid.cpp
             JniUtils.cpp
             DataTypeUtils.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ${TARGET}
        socket_message_sdk
        ${log-lib} )

# 配置要编译的模块
set(SOCKET_MESSAGE_SDK_PATH ${CMAKE_SOURCE_DIR}/../../../../../socket_message_client)
add_subdirectory(${SOCKET_MESSAGE_SDK_PATH}/socket_message_sdk socket_message_sdk.out)
target_include_directories(${TARGET} PRIVATE ${SOCKET_MESSAGE_SDK_PATH}/include)