add_executable(char_set_tests
   test_any.cpp
   test_delegated.cpp
   test_not.cpp
   test_or.cpp
   test_range.cpp
   test_set.cpp
   test_single.cpp
)

target_link_libraries(char_set_tests abu_test_main gtest)
add_test(char_set_tests char_set_tests)

set_target_properties(char_set_tests PROPERTIES FOLDER "tests")
