// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"

// Windows brain damage
#if defined(IN)
    #undef IN
#endif

namespace Unicorn {

    // The default value is always listed first

    enum class Bidi_Class: uint8_t {
        Default, AL, AN, B, BN, CS, EN, ES, ET, FSI, L, LRE, LRI, LRO, NSM,
        ON, PDF, PDI, R, RLE, RLI, RLO, S, WS
    };

    enum class East_Asian_Width: uint8_t {
        N, A, F, H, Na, W
    };

    enum class Grapheme_Cluster_Break: uint8_t {
        Other, Control, CR, EOT, Extend, L, LF, LV, LVT, Prepend,
        Regional_Indicator, SOT, SpacingMark, T, V
    };

    enum class Hangul_Syllable_Type: uint8_t {
        NA, L, LV, LVT, T, V
    };

    enum class Indic_Positional_Category: uint8_t {
        NA, Bottom, Bottom_And_Right, Left, Left_And_Right, Overstruck, Right,
        Top, Top_And_Bottom, Top_And_Bottom_And_Right, Top_And_Left,
        Top_And_Left_And_Right, Top_And_Right, Visual_Order_Left
    };

    enum class Indic_Syllabic_Category: uint8_t {
        Other, Avagraha, Bindu, Brahmi_Joining_Number, Cantillation_Mark,
        Consonant, Consonant_Dead, Consonant_Final, Consonant_Head_Letter,
        Consonant_Killer, Consonant_Medial, Consonant_Placeholder,
        Consonant_Preceding_Repha, Consonant_Prefixed, Consonant_Subjoined,
        Consonant_Succeeding_Repha, Consonant_With_Stacker, Gemination_Mark,
        Invisible_Stacker, Joiner, Modifying_Letter, Non_Joiner, Nukta,
        Number, Number_Joiner, Pure_Killer, Register_Shifter,
        Syllable_Modifier, Tone_Letter, Tone_Mark, Virama, Visarga, Vowel,
        Vowel_Dependent, Vowel_Independent
    };

    enum class Joining_Group: uint8_t {
        No_Joining_Group, Ain, Alaph, Alef, Beh, Beth, Burushaski_Yeh_Barree,
        Dal, Dalath_Rish, E, Farsi_Yeh, Fe, Feh, Final_Semkath, Gaf, Gamal,
        Hah, He, Heh, Heh_Goal, Heth, Kaf, Kaph, Khaph, Knotted_Heh, Lam,
        Lamadh, Manichaean_Aleph, Manichaean_Ayin, Manichaean_Beth,
        Manichaean_Daleth, Manichaean_Dhamedh, Manichaean_Gimel,
        Manichaean_Heth, Manichaean_Kaph, Manichaean_Lamedh, Manichaean_Mem,
        Manichaean_Nun, Manichaean_Pe, Manichaean_Samekh, Manichaean_Teth,
        Manichaean_Thamedh, Manichaean_Waw, Manichaean_Yodh, Manichaean_Zayin,
        Manichaean_Sadhe, Manichaean_Qoph, Manichaean_Resh, Manichaean_Taw,
        Manichaean_One, Manichaean_Five, Manichaean_Ten, Manichaean_Twenty,
        Manichaean_Hundred, Meem, Mim, Noon, Nun, Nya, Pe, Qaf, Qaph, Reh,
        Reversed_Pe, Rohingya_Yeh, Sad, Sadhe, Seen, Semkath, Shin,
        Straight_Waw, Swash_Kaf, Syriac_Waw, Tah, Taw, Teh_Marbuta,
        Teh_Marbuta_Goal, Teth, Waw, Yeh, Yeh_Barree, Yeh_With_Tail, Yudh,
        Yudh_He, Zain, Zhain
    };

    enum class Joining_Type: uint8_t {
        Default, Dual_Joining, Join_Causing, Left_Joining, Non_Joining,
        Right_Joining, Transparent
    };

    enum class Line_Break: uint8_t {
        XX, AI, AL, B2, BA, BB, BK, CB, CJ, CL, CM, CP, CR, EX, GL, H2, H3,
        HL, HY, ID, IN, IS, JL, JT, JV, LF, NL, NS, NU, OP, PO, PR, QU, RI,
        SA, SG, SP, SY, WJ, ZW
    };

    enum class Numeric_Type: uint8_t {
        None, Decimal, Digit, Numeric
    };

    enum class Sentence_Break: uint8_t {
        Other, ATerm, Close, CR, EOT, Extend, Format, LF, Lower, Numeric,
        OLetter, SContinue, Sep, SOT, Sp, STerm, Upper
    };

    enum class Word_Break: uint8_t {
        Other, ALetter, CR, Double_Quote, EOT, Extend, ExtendNumLet, Format,
        Hebrew_Letter, Katakana, LF, MidLetter, MidNum, MidNumLet, Newline,
        Numeric, Regional_Indicator, Single_Quote, SOT
    };

}
