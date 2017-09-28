add_executable(operations_test
   test_make_parser.cpp
)

target_link_libraries(operations_test abu_test_main gtest)
add_test(operations_test operations_test)

set_target_properties(operations_test PROPERTIES FOLDER "tests")
