#pragma once

#include "unicorn/core.hpp"

namespace RS::Unicorn {

    RS_ENUM(NormalizationForm, int, 1, NFC, NFD, NFKC, NFKD)

    Ustring normalize(const Ustring& src, NormalizationForm form);
    void normalize_in(Ustring& src, NormalizationForm form);

}
