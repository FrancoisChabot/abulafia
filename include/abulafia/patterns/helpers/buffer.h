//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_HELPERS_BUFFER_H_
#define ABULAFIA_PATTERNS_HELPERS_BUFFER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/patterns/helpers/collection_assign_wrapper.h"

namespace ABULAFIA_NAMESPACE {
// if PAT_T appends:
//   if DST is collection:
//     push(vbuf)
//   else:
//     if PAT_T is stable:
//       wrap()
//     else:
//       dst.append(vbuf)
// else
//   if PAT_T is atomic
//     wrap()
//   else
//     push(buf)

namespace buf_ {

enum class CollectionAdapter {
  WRAP,
  PUSH_VAL,
  APPEND_COLLECTION,
  BAD_MATCH,
};

template <typename DST_T, typename PAT_T>
constexpr CollectionAdapter choose_collection_adapter() {
  if (is_same<Nil, DST_T>::value) {
    // If we are writing to nil, use the lightest adapter, which is wrap.
    return CollectionAdapter::WRAP;
  } else if (pattern_traits<PAT_T, void>::APPENDS_DST) {
    // If the child pattern is a loop pattern.
    if (is_collection<typename DST_T::value_type>()) {
      // And the destination is a collection of collection
      // Build a collection and append it on success
      return CollectionAdapter::PUSH_VAL;
    } else {
      // The destination is a collection of values
      if (pattern_traits<PAT_T, void>::STABLE_APPENDS) {
        // If the child pattern is stable, just push as we go
        return CollectionAdapter::WRAP;
      } else {
        // If the child pattern is not stable, build the collection
        // first, and concatenate it to dst on sucess.
        return CollectionAdapter::APPEND_COLLECTION;
      }
    }
  } else {
    // The child pattern is a value pattern
    if (pattern_traits<PAT_T, void>::ATOMIC) {
      // If it's atomic, then just write to the destination
      return CollectionAdapter::WRAP;
    } else {
      // If it's not atomic, we need to store the value in a buffer, and push it
      // on success.
      return CollectionAdapter::PUSH_VAL;
    }
  }

  return CollectionAdapter::BAD_MATCH;
}

// This is a hack for MSVC 2017, which has trouble with constexpr function
// operand for enable_of_t.
template <typename DST_T, typename PAT_T>
struct ChosenCollectionAdapter
    : public std::integral_constant<CollectionAdapter,
                                    choose_collection_adapter<DST_T, PAT_T>()> {
};

template <typename CTX_T, typename DST_T, typename PAT_T,
          typename Enable = void>
struct CollectionParserAdapter;

// CollectionAdapter::WRAP
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::WRAP>> {
  using assign_wrapper_t = CollectionAssignWrapper<DST_T>;
  using child_parser_t = Parser<CTX_T, assign_wrapper_t, PAT_T>;

  CollectionParserAdapter(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : child_parser_(ctx, force_lvalue(assign_wrapper_t(dst)), pat) {}

  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto wrapped = assign_wrapper_t(dst);
    return child_parser_.consume(ctx, wrapped, pat);
  }

  child_parser_t child_parser_;
};

// CollectionAdapter::PUSH_VAL
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::PUSH_VAL>> {
  using buffer_t = typename DST_T::value_type;
  using child_parser_t = Parser<CTX_T, buffer_t, PAT_T>;

  CollectionParserAdapter(CTX_T& ctx, DST_T&, PAT_T const& pat)
      : child_parser_(ctx, buffer_, pat) {}

  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto result = child_parser_.consume(ctx, buffer_, pat);
    if (result == result::SUCCESS) {
      dst.emplace_back(buffer_);
    }
    return result;
  }

  buffer_t buffer_;
  child_parser_t child_parser_;
};

// CollectionAdapter::APPEND_COLLECTION
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::APPEND_COLLECTION>> {
  using buffer_t = std::vector<typename DST_T::value_type>;
  using child_parser_t = Parser<CTX_T, buffer_t, PAT_T>;

  CollectionParserAdapter(CTX_T& ctx, DST_T&, PAT_T const& pat)
      : child_parser_(ctx, buffer_, pat) {}

  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto result = child_parser_.consume(ctx, buffer_, pat);
    if (result == result::SUCCESS) {
      dst.insert(dst.end(), buffer_.begin(), buffer_.end());
    }
    return result;
  }

  buffer_t buffer_;
  child_parser_t child_parser_;
};

}  // namespace buf_
}  // namespace ABULAFIA_NAMESPACE
#endif