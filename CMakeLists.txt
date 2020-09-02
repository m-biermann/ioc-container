cmake_minimum_required(VERSION 3.16)
project(ioc)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)

add_library(ioc INTERFACE)
target_include_directories(ioc INTERFACE include/)

add_subdirectory(test)