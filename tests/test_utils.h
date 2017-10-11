#ifndef ABULAFIA_TEST_UTILS_H_
#define ABULAFIA_TEST_UTILS_H_

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

// This will run a few standardized tests, preventing test writers from having
// to manually re-implement all these permutations.
template <typename PAT_T, typename DST_T>
void testPatternSuccess(std::string const& data, PAT_T const& pat,
                        DST_T const& expected_value) {
  // Test parsing into Nil
  {
    auto status = abu::parse(data, pat);
    EXPECT_EQ(status, abu::Result::SUCCESS);
  }

  // Test parsing into Value
  {
    DST_T dst{};
    auto status = abu::parse(data, pat, dst);
    EXPECT_EQ(status, abu::Result::SUCCESS);
    EXPECT_EQ(dst, expected_value);
  }

  // Test parsing into Value, wrapped in recur
  {
    abu::RecurMemoryPool pool;
    abu::Recur<struct as_recur_t> as_recur(pool);
    ABU_Recur_define(as_recur, as_recur_t, abu::make_pattern(pat));

    DST_T dst{};
    auto status = abu::parse(data, as_recur, dst);
    EXPECT_EQ(status, abu::Result::SUCCESS);
    EXPECT_EQ(dst, expected_value);
  }

  // Test drip-feeding one character at a time
  // First into Nil
  {
    auto parser = abu::make_parser<std::string>(pat);

    abu::Result status = abu::Result::PARTIAL;
    for (auto c : data) {
      parser.data().add_buffer(std::string(&c, 1));
      status = parser.consume();
      if (status != abu::Result::PARTIAL) {
        break;
      }
    }
    if (status == abu::Result::PARTIAL) {
      parser.data().add_buffer(std::string(), abu::IsFinal::FINAL);
      status = parser.consume();
    }
    EXPECT_EQ(status, abu::Result::SUCCESS);
  }

  // Then into a DST
  {
    DST_T dst{};
    auto parser = abu::make_parser<std::string>(pat, dst);
    abu::Result status = abu::Result::PARTIAL;
    for (auto c : data) {
      parser.data().add_buffer(std::string(&c, 1));
      status = parser.consume();

      if (status != abu::Result::PARTIAL) {
        break;
      }
    }
    if (status == abu::Result::PARTIAL) {
      parser.data().add_buffer(std::string(), abu::IsFinal::FINAL);
      status = parser.consume();
    }
    EXPECT_EQ(status, abu::Result::SUCCESS);
    EXPECT_EQ(dst, expected_value);
  }
}

template <typename DST_T, typename PAT_T>
void testPatternFailure(std::string const& data, PAT_T const& pat) {
  // Test parsing into Nil
  {
    auto status = abu::parse(data, pat);
    EXPECT_EQ(status, abu::Result::FAILURE);
  }

  // Test parsing into Value
  {
    DST_T dst;
    auto status = abu::parse(data, pat, dst);
    EXPECT_EQ(status, abu::Result::FAILURE);
  }

  // Test drip-feeding one character at a time
  // First into Nil
  {
    auto parser = abu::make_parser<std::string>(pat);

    for (auto c : data) {
      parser.data().add_buffer(std::string(&c, 1));
      auto status = parser.consume();
      EXPECT_NE(status, abu::Result::SUCCESS);
      if (status != abu::Result::PARTIAL) {
        break;
      }
    }
  }

  // Then into a DST
  {
    DST_T dst{};
    auto parser = abu::make_parser<std::string>(pat, dst);

    for (auto c : data) {
      parser.data().add_buffer(std::string(&c, 1));
      auto status = parser.consume();
      EXPECT_NE(status, abu::Result::SUCCESS);
      if (status != abu::Result::PARTIAL) {
        break;
      }
    }
  }
}
#endif