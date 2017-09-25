#include "abulafia/contexts/single_forward.h"

char const* string = "aaa";
auto tmp = abu::SingleForwardContext<char const*>(string, string + 3);