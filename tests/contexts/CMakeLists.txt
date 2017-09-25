add_executable(contexts_tests
   test_single_forward.cpp
   test_skipper.cpp
)

target_link_libraries(contexts_tests gtest_main)
add_test(contexts_tests contexts_tests)

set_target_properties(contexts_tests PROPERTIES FOLDER "tests")
