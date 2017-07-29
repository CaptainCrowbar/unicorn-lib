#include "unicorn/string.hpp"

namespace RS::Unicorn {

    bool str_is_east_asian(const U8string& str) {
        auto range = utf_range(str);
        for (char32_t c: range) {
            auto w = east_asian_width(c);
            if (w == East_Asian_Width::F || w == East_Asian_Width::H || w == East_Asian_Width::W)
                return true;
        }
        return false;
    }

    bool str_starts_with(const U8string& str, const U8string& prefix) noexcept {
        return str.size() >= prefix.size() && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
    }

    bool str_ends_with(const U8string& str, const U8string& suffix) noexcept {
        return str.size() >= suffix.size() && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

}
