cmake_minimum_required(VERSION 2.8)

project(simpleraytracer)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    if(CMAKE_BUILD_TYPE MATCHES "Release")
        message("O2 enabled")
        add_compile_options(/O2 /Og)
    endif()
endif()

set(SOURCE "src/main.cpp")

include_directories("include")

add_executable(simpleraytracer ${SOURCE})