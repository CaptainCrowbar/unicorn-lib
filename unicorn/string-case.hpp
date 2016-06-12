// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include <string>

namespace Unicorn {

    u8string str_uppercase(const u8string& str);
    u8string str_lowercase(const u8string& str);
    u8string str_titlecase(const u8string& str);
    u8string str_casefold(const u8string& str);
    void str_uppercase_in(u8string& str);
    void str_lowercase_in(u8string& str);
    void str_titlecase_in(u8string& str);
    void str_casefold_in(u8string& str);

}
