add_executable(pattern_tests
   test_character.cpp
   test_repeat.cpp
   test_uint.cpp
)

target_link_libraries(pattern_tests abu_test_main gtest)
add_test(pattern_tests pattern_tests)

set_target_properties(pattern_tests PROPERTIES FOLDER "tests")
