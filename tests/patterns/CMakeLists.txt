add_executable(pattern_tests
   test_action.cpp
   test_alternative.cpp
   test_char_literal.cpp
   test_character.cpp
   test_digit_values.cpp
   test_eoi.cpp
   test_except.cpp
   test_int.cpp
   test_list.cpp
   test_not.cpp
   test_pass_and_fail.cpp
   test_optional.cpp
   test_recur.cpp
   test_repeat.cpp
   test_sequence.cpp
   test_symbol.cpp
   test_uint.cpp
)

target_link_libraries(pattern_tests abu_test_main gtest)
add_test(pattern_tests pattern_tests)

set_target_properties(pattern_tests PROPERTIES FOLDER "tests")
