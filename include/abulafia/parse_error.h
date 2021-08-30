//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSE_ERROR_H_INCLUDED
#define ABULAFIA_PARSE_ERROR_H_INCLUDED

#include <stdexcept>

namespace abu {

class parse_error : public std::exception {};

}  // namespace abu
#endif