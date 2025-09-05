cmake_minimum_required(VERSION 3.20)
project(Winamp-Song-Deleter-Plugin)

set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/WinampSDK
)

add_library(Winamp-Song-Deleter-Plugin SHARED
    src/plugin.cpp
    src/resource.h
    src/WinampSongDeleterPlugin.rc
)

set_target_properties(Winamp-Song-Deleter-Plugin PROPERTIES
    OUTPUT_NAME "gen_WinampSongDeleter"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    SUFFIX ".dll"
    PREFIX ""
)

if(MSVC)
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /I\"${CMAKE_SOURCE_DIR}/resources\"")
endif()

target_link_libraries(Winamp-Song-Deleter-Plugin PRIVATE Shell32 User32)
