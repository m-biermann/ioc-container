# 'Boost_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable (${PROJECT_NAME}Tests container/ArgTests.cpp container/argStructs.h container/structs.h container/SingletonInstanceTests.cpp)# container/SingletonFactoryTests.cpp)# container/ClassFactoryTests.cpp)
target_link_libraries (${PROJECT_NAME}Tests PRIVATE ${Boost_LIBRARIES} ${PROJECT_NAME})
target_include_directories (${PROJECT_NAME}Tests PRIVATE ${Boost_INCLUDE_DIRS})

add_test (NAME ${PROJECT_NAME}Tests COMMAND ${PROJECT_NAME}Tests)