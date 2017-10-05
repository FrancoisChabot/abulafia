add_executable(datasource_tests
   test_container_sequence.cpp
   test_single_forward.cpp
)

target_link_libraries(datasource_tests abu_test_main gtest)
add_test(datasource_tests datasource_tests)

set_target_properties(datasource_tests PROPERTIES FOLDER "tests")
