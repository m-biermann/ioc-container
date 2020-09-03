# 'Boost_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
add_executable (iocTests container/SingletonInstanceTests.cpp container/SingletonFactoryTests.cpp container/structs.h container/ClassFactoryTests.cpp container/ArgTests.cpp container/argStructs.h)
target_link_libraries (iocTests PRIVATE ${Boost_LIBRARIES} ${PROJECT_NAME}::ioc)
target_include_directories(iocTests PRIVATE ${Boost_INCLUDE_DIRS})

add_test(NAME iocTests COMMAND iocTests)