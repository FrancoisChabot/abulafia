
add_library( header_sanitizer
  char_set/any.cpp
  char_set/char_set.cpp
  char_set/delegated.cpp
  char_set/not.cpp
  char_set/operators.cpp
  char_set/or.cpp
  char_set/range.cpp
  char_set/set.cpp
  char_set/single.cpp

  context/single_forward.cpp

  operations/parse.cpp

  patterns/helpers/buffer.cpp
  patterns/helpers/collection_assign_wrapper.cpp

  patterns/binary/except.cpp
  patterns/binary/list.cpp

  patterns/leaf/char_literal.cpp
  patterns/leaf/char_symbol.cpp
  patterns/leaf/character.cpp
  patterns/leaf/eoi.cpp
  patterns/leaf/fail.cpp
  patterns/leaf/pass.cpp
  patterns/leaf/string_literal.cpp
  patterns/leaf/string_symbol.cpp

  patterns/leaf/numeric/digit_values.cpp
  patterns/leaf/numeric/int.cpp
  patterns/leaf/numeric/uint.cpp

  patterns/nary/alternative.cpp
  patterns/nary/sequence.cpp
  patterns/nary/nary.cpp

  patterns/unary/attr_cast.cpp
  patterns/unary/not.cpp
  patterns/unary/repeat.cpp
  
  patterns/operators.cpp

  support/function_traits.cpp
  support/nil.cpp
  support/type_traits.cpp

  pattern.cpp
  result.cpp
  traits.cpp
)

set_target_properties(header_sanitizer PROPERTIES FOLDER "tests")
