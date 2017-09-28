//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ABULAFIA_H_
#define ABULAFIA_ABULAFIA_H_

#include "abulafia/config.h"

// Character sets
#include "abulafia/char_set/char_set.h"

#include "abulafia/char_set/any.h"
#include "abulafia/char_set/delegated.h"
#include "abulafia/char_set/not.h"
#include "abulafia/char_set/or.h"
#include "abulafia/char_set/range.h"
#include "abulafia/char_set/set.h"
#include "abulafia/char_set/single.h"

#include "abulafia/char_set/operators.h"

// Parsing Contexts

#include "abulafia/contexts/container_sequence.h"
#include "abulafia/contexts/single_forward.h"
#include "abulafia/contexts/skipper_adapter.h"

// Operations
#include "abulafia/operations/make_parser.h"
#include "abulafia/operations/parse.h"

// Patterns
#include "abulafia/patterns/recur.h"
#include "abulafia/patterns/with_skipper.h"

#include "abulafia/patterns/binary/except.h"
#include "abulafia/patterns/binary/list.h"

#include "abulafia/patterns/leaf/char_literal.h"
#include "abulafia/patterns/leaf/char_symbol.h"
#include "abulafia/patterns/leaf/eoi.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/patterns/leaf/pass.h"
#include "abulafia/patterns/leaf/string_literal.h"
#include "abulafia/patterns/leaf/string_symbol.h"

#include "abulafia/patterns/leaf/numeric/digit_values.h"
#include "abulafia/patterns/leaf/numeric/int.h"
#include "abulafia/patterns/leaf/numeric/uint.h"

#include "abulafia/patterns/nary/alternative.h"
#include "abulafia/patterns/nary/sequence.h"

#include "abulafia/patterns/unary/action.h"
#include "abulafia/patterns/unary/attr_cast.h"
#include "abulafia/patterns/unary/not.h"
#include "abulafia/patterns/unary/repeat.h"

#include "abulafia/patterns/operators.h"

#include "abulafia/patterns.h"

#endif