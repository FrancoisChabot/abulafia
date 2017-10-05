//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

struct FakeData {
  using const_iterator = char*;
  using value_type = char;
  char* begin() { return nullptr; }
  char* end() { return (char*)1; }

  bool* tgt;

  FakeData(bool* t) : tgt(t) { *t = true; }
  ~FakeData() { *tgt = false; }
};

TEST(test_container_sequence, take_ownership) {
  bool alive = false;
  {
    ContainerSequenceDataSource<FakeData> ctx;
    auto data = std::make_unique<FakeData>(&alive);

    EXPECT_TRUE(alive);
    ctx.add_buffer(std::move(data), IsFinal::FINAL);
    data.reset();
    EXPECT_TRUE(alive);
  }
  EXPECT_FALSE(alive);
}

TEST(test_container_sequence, handles_shared_ptr) {
  bool alive = false;
  {
    ContainerSequenceDataSource<FakeData> ctx;
    auto data = std::make_shared<FakeData>(&alive);
    EXPECT_TRUE(alive);
    ctx.add_buffer(data, IsFinal::FINAL);
    data.reset();
    EXPECT_TRUE(alive);
  }
  EXPECT_FALSE(alive);
}

TEST(test_container_sequence, handles_object) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("abcd", IsFinal::FINAL);
  EXPECT_EQ('a', ctx.next());
}

TEST(test_container_sequence, handles_multiple_buffers) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("ab");
  ctx.add_buffer("c");
  ctx.add_buffer("de", IsFinal::FINAL);

  EXPECT_EQ('a', ctx.next());
  ctx.advance();
  EXPECT_FALSE(ctx.empty());

  EXPECT_EQ('b', ctx.next());
  ctx.advance();
  EXPECT_FALSE(ctx.empty());

  EXPECT_EQ('c', ctx.next());
  ctx.advance();
  EXPECT_FALSE(ctx.empty());

  EXPECT_EQ('d', ctx.next());
  ctx.advance();
  EXPECT_FALSE(ctx.empty());

  EXPECT_EQ('e', ctx.next());
  ctx.advance();
  EXPECT_TRUE(ctx.empty());
}

TEST(test_container_sequence, perform_rollback_to_start) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.advance();
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_container_sequence, perform_rollback_to_middle) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.advance();
  ctx.advance();

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.advance();
  ctx.commit_rollback();
  EXPECT_EQ('3', ctx.next());
}

TEST(test_container_sequence, perform_rollback_from_empty) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.prepare_rollback();
  while (!ctx.empty()) {
    ctx.advance();
  }
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_container_sequence, immediate_rollback) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.prepare_rollback();
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_container_sequence, perform_multiple_rollback) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.prepare_rollback();
  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.prepare_rollback();

  ctx.advance();
  ctx.advance();

  EXPECT_EQ('5', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('3', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_container_sequence, cancel_rollback) {
  ContainerSequenceDataSource<std::string> ctx;

  ctx.add_buffer("12");
  ctx.add_buffer("34");
  ctx.add_buffer("56");
  ctx.add_buffer("78");
  ctx.add_buffer("90", IsFinal::FINAL);

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.prepare_rollback();

  ctx.advance();
  ctx.advance();

  EXPECT_EQ('5', ctx.next());
  ctx.cancel_rollback();
  EXPECT_EQ('5', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}
