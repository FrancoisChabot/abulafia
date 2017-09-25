#include "abulafia/char_set/delegated.h"

auto delegate = abu::char_set::delegated([](char) { return false; });