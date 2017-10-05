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

// Parsing Contexts

#include "abulafia/data_source/container_sequence.h"
#include "abulafia/data_source/single_forward.h"

// Operations
#include "abulafia/operations/make_parser.h"
#include "abulafia/operations/parse.h"

// Patterns
#include "abulafia/patterns/all.h"

// Parsers
#include "abulafia/parsers/coroutine/all.h"

#endif