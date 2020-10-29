cmake_minimum_required(VERSION 3.16)
project(ioc-container)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED PACKAGE_NAME)
    set(PACKAGE_NAME mabiphmo)
endif()

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(GNUInstallDirs)

if(DEFINED ENV{CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX $ENV{CMAKE_INSTALL_PREFIX})
endif()

if(DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
endif()

if(DEFINED ENV{Boost_DEBUG})
    set(Boost_DEBUG $ENV{Boost_DEBUG})
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PACKAGE_NAME}Targets
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mabiphmo TYPE INCLUDE)

enable_testing()

add_subdirectory(test)