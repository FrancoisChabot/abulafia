//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINES_ALL_H_
#define ABULAFIA_PARSERS_COROUTINES_ALL_H_

#include "abulafia/parsers/coroutine/leaf/numeric/int.h"
#include "abulafia/parsers/coroutine/leaf/numeric/uint.h"

#include "abulafia/parsers/coroutine/leaf/char_symbol.h"
#include "abulafia/parsers/coroutine/leaf/character.h"
#include "abulafia/parsers/coroutine/leaf/eoi.h"
#include "abulafia/parsers/coroutine/leaf/fail.h"
#include "abulafia/parsers/coroutine/leaf/pass.h"
#include "abulafia/parsers/coroutine/leaf/string_literal.h"
#include "abulafia/parsers/coroutine/leaf/string_symbol.h"

#include "abulafia/parsers/coroutine/binary/except.h"
#include "abulafia/parsers/coroutine/binary/list.h"

#include "abulafia/parsers/coroutine/nary/alternative.h"
#include "abulafia/parsers/coroutine/nary/sequence.h"

#include "abulafia/parsers/coroutine/unary/action.h"
#include "abulafia/parsers/coroutine/unary/discard.h"
#include "abulafia/parsers/coroutine/unary/not.h"
#include "abulafia/parsers/coroutine/unary/optional.h"
#include "abulafia/parsers/coroutine/unary/repeat.h"

#include "abulafia/parsers/coroutine/recur.h"
#include "abulafia/parsers/coroutine/with_skipper.h"

#endif