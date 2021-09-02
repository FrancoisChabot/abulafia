//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABU_TEST_UTILS_H_INCLUDED
#define ABU_TEST_UTILS_H_INCLUDED

#include "abulafia/abulafia.h"

namespace {
template <auto pattern_like,
          abu::Policies auto policies = abu::default_policies,
          typename Data>
bool abu_test_match(const Data& data) {
  constexpr auto pattern = abu::as_pattern(pattern_like);
  bool result = true;

  // Default immediate matcher
  result = match<pattern>(data) && result;

  // Coroutine matcher: single pass
  {
    auto b = std::ranges::begin(data);
    auto e = std::ranges::end(data);
    abu::data_chunk data{b, e};
    abu::coro::matcher<pattern, policies, decltype(data)> matcher(data);

    auto tmp = matcher.on_tokens(data);
    if (tmp.is_partial()) {
      tmp = matcher.on_end(data);
    }

    result = result && tmp.is_success();
  }

  // Coroutine matcher: drip-feed
  {
    auto b = std::ranges::begin(data);
    auto e = std::ranges::end(data);

    using token_type = std::iter_value_t<decltype(b)>;
    using chunk_type = std::vector<token_type>;

    abu::data_feed<chunk_type> data{};
    abu::coro::matcher<pattern, policies, decltype(data)> matcher(data);
    abu::op_result status = abu::partial_result;
    while (status.is_partial() && b != e) {
      auto chunk = std::make_shared<chunk_type>(b, std::next(b));

      data.add(chunk);
      status = matcher.on_tokens(data);
      b++;
    }

    if (status.is_partial()) {
      status = matcher.on_end(data);
    }

    result = result && status.is_success();
  }

  // Coroutine matcher: chunks
  if (std::ranges::size(data) > 2) {
    auto b = std::ranges::begin(data);
    auto e = std::ranges::end(data);

    using token_type = std::iter_value_t<decltype(b)>;
    using chunk_type = std::vector<token_type>;

    auto midpoint = std::next(b, std::ranges::size(data) / 2);

    chunk_type chunk_2(midpoint, e);

    abu::data_feed<chunk_type> data{};
    abu::coro::matcher<pattern, policies, decltype(data)> matcher(data);

    data.add(std::make_shared<chunk_type>(b, midpoint));
    data.add(std::make_shared<chunk_type>(midpoint, e));

    auto tmp = matcher.on_tokens(data);
    if (tmp.is_partial()) {
      tmp = matcher.on_end(data);
    }

    result = result && tmp.is_success();
  }

  return result;
}

template <auto pattern, typename Data, typename Expected>
bool abu_test_parses_as(const Data& data, const Expected& expected) {
  bool result = abu_test_match<pattern>(data);

  // Default immediate parser
  result = parse<pattern>(data) == expected && result;

  return result;
}

template <auto pattern, typename Data>
bool abu_test_no_match(const Data& data) {
  bool result = true;

  // Default immediate matcher
  result = !match<pattern>(data) && result;

  return result;
}

template <auto pattern, typename Data>
bool abu_test_parse_fail(const Data& data) {
  bool result = abu_test_no_match<pattern>(data);

  // Default immediate parser
  try {
    (void)parse<pattern>(data);
    result = false;
  } catch (abu::no_match_error&) {
    // This is what we want.
  } catch (...) {
    result = false;
  }

  return result;
}

#define ABU_EXPECT_MATCH(pat, data) \
  GTEST_EXPECT_TRUE(abu_test_match<(pat)>((data)))
#define ABU_EXPECT_PARSES_AS(pat, data, expected) \
  GTEST_EXPECT_TRUE(abu_test_parses_as<(pat)>((data), (expected)))
#define ABU_EXPECT_NO_MATCH(pat, data) \
  GTEST_EXPECT_TRUE(abu_test_no_match<(pat)>((data)))
#define ABU_EXPECT_PARSE_FAIL(pat, data) \
  GTEST_EXPECT_TRUE(abu_test_parse_fail<(pat)>((data)))

}  // namespace

#endif