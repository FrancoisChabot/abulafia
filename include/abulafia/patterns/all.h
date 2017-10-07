//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULABIA_PATTERNS_ALL_H_
#define ABULABIA_PATTERNS_ALL_H_

// Patterns
#include "abulafia/patterns/recur.h"
#include "abulafia/patterns/with_skipper.h"

#include "abulafia/patterns/binary/except.h"
#include "abulafia/patterns/binary/list.h"

#include "abulafia/patterns/leaf/char_literal.h"
#include "abulafia/patterns/leaf/char_symbol.h"
#include "abulafia/patterns/leaf/character.h"
#include "abulafia/patterns/leaf/eoi.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/patterns/leaf/pass.h"
#include "abulafia/patterns/leaf/string_literal.h"
#include "abulafia/patterns/leaf/string_symbol.h"

#include "abulafia/patterns/leaf/numeric/int.h"
#include "abulafia/patterns/leaf/numeric/uint.h"

#include "abulafia/patterns/nary/alternative.h"
#include "abulafia/patterns/nary/sequence.h"

#include "abulafia/patterns/unary/action.h"
#include "abulafia/patterns/unary/discard.h"
#include "abulafia/patterns/unary/lexeme.h"
#include "abulafia/patterns/unary/not.h"
#include "abulafia/patterns/unary/optional.h"
#include "abulafia/patterns/unary/repeat.h"

#endif