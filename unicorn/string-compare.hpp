// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <string>

namespace Unicorn {

    int str_compare_3way(const u8string& lhs, const u8string& rhs);
    bool str_icase_compare(const u8string& lhs, const u8string& rhs) noexcept;
    bool str_icase_equal(const u8string& lhs, const u8string& rhs) noexcept;
    bool str_natural_compare(const u8string& lhs, const u8string& rhs) noexcept;

}
