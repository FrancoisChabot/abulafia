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

// This is used to test both forward_iterator and input_iterator behavior all
// at once.
template <std::forward_iterator T>
struct fake_input_iterator {
  using iterator_tag = std::input_iterator_tag;
  using difference_type = std::iter_difference_t<T>;
  using value_type = std::iter_value_t<T>;

  constexpr auto operator*() const { return *ite; }
  constexpr fake_input_iterator& operator++() {
    ++ite;
    return *this;
  }

  constexpr void operator++(int) { ite++; }
  constexpr bool operator==(const fake_input_iterator& rhs) const {
    return ite == rhs.ite;
  }
  T ite;
};

static_assert(
    std::input_iterator<fake_input_iterator<std::vector<int>::iterator>>);
static_assert(
    !std::forward_iterator<fake_input_iterator<std::vector<int>::iterator>>);

template <std::ranges::forward_range T>
struct fake_input_range {
  auto begin() const { return fake_input_iterator{std::ranges::begin(range)}; }
  auto end() const { return fake_input_iterator{std::ranges::end(range)}; }

  T& range;
};

static_assert(std::ranges::input_range<fake_input_range<std::vector<int>>>);
static_assert(!std::ranges::forward_range<fake_input_range<std::vector<int>>>);

template <auto pattern_like,
          abu::Policies auto policies = abu::default_policies,
          std::ranges::forward_range Data>
bool abu_test_match(const Data& data) {
  constexpr auto pattern = abu::as_pattern(pattern_like);
  bool result = true;

  // Default simple-pass matcher
  result = match<pattern>(data) && result;
  result = match<pattern>(fake_input_range{data}) && result;

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

    result =  tmp.is_success() && result ;
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
      auto chunk = chunk_type{b, std::next(b)};

      abu_assume(chunk.size() == 1);
      data.add(std::move(chunk));
      status = matcher.on_tokens(data);
      b++;
    }

    if (status.is_partial()) {
      status = matcher.on_end(data);
    }

    result = status.is_success() && result;
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

    data.add(chunk_type(b, midpoint));
    data.add(chunk_type(midpoint, e));

    auto tmp = matcher.on_tokens(data);
    if (tmp.is_partial()) {
      tmp = matcher.on_end(data);
    }

    result = tmp.is_success() && result;
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