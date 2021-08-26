add_executable(abulafia_tests
    test_result.cpp
    test_tok.cpp
)

target_link_libraries(abulafia_tests PRIVATE gtest_main abulafia)
target_code_coverage(abulafia_tests EXCLUDE *gtest*)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(abulafia_tests PRIVATE /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(abulafia_tests PRIVATE -Wall -Wextra -pedantic -Werror -ftemplate-backtrace-limit=20)
endif()

add_test(abulafia_tests abulafia_tests)