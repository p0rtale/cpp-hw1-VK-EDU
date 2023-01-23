add_executable(ustring_test unit/ustring_test.cpp)
target_link_libraries(ustring_test ustring_lib GTest::gtest)

add_test(NAME    ustring_test 
         COMMAND ustring_test)
