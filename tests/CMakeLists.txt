add_library(abu_test_main test_main.cpp)

add_subdirectory(char_set)
add_subdirectory(contexts)
add_subdirectory(patterns)
add_subdirectory(operations)
add_subdirectory(documentation)

SET(HEADER_SAN_SRC "")

# Create the header sanitizer targets
foreach(HEADER_FILE ${ABULAFIA_HEADERS})
  get_filename_component(TGT_DIR ${HEADER_FILE} DIRECTORY)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TGT_DIR})
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${HEADER_FILE}.cpp "#include \"${HEADER_FILE}\"")

  list(APPEND HEADER_SAN_SRC ${CMAKE_CURRENT_BINARY_DIR}/${HEADER_FILE}.cpp)
endforeach()

add_library(header_sanitizer ${HEADER_SAN_SRC})

set_target_properties(abu_test_main PROPERTIES FOLDER "tests")
set_target_properties(header_sanitizer PROPERTIES FOLDER "tests")