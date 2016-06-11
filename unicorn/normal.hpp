#pragma once

#include "unicorn/core.hpp"
#include <string>

namespace Unicorn {

    PRI_ENUM(NormalizationForm, 1, NFC, NFD, NFKC, NFKD)

    u8string normalize(const u8string& src, NormalizationForm form);
    void normalize_in(u8string& src, NormalizationForm form);

}
