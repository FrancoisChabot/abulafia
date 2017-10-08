add_executable(documentation_test
   test_readme.cpp
)

target_link_libraries(documentation_test abu_test_main gtest)
add_test(documentation_test documentation_test)

set_target_properties(documentation_test PROPERTIES FOLDER "tests")

