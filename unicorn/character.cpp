#include "unicorn/character.hpp"
#include "unicorn/iso-script-names.hpp"
#include "unicorn/ucd-tables.hpp"
#include <array>
#include <cstdint>
#include <iterator>
#include <unordered_map>
#include <zlib.h>

using namespace std::literals;

namespace Unicorn {

    namespace {

        static constexpr uint32_t sbase = 0xac00;
        static constexpr uint32_t lbase = 0x1100;
        static constexpr uint32_t vbase = 0x1161;
        static constexpr uint32_t tbase = 0x11a7;
        static constexpr uint32_t tcount = 28;
        static constexpr uint32_t ncount = 588;

        size_t hangul_decomposition(char32_t c, char32_t* dst) {
            auto type = hangul_syllable_type(c);
            if (type != Hangul_Syllable_Type::LV && type != Hangul_Syllable_Type::LVT)
                return 0;
            uint32_t sindex = c - sbase;
            if (type == Hangul_Syllable_Type::LV) {
                uint32_t lindex = sindex / ncount;
                uint32_t vindex = (sindex % ncount) / tcount;
                dst[0] = lbase + lindex;
                dst[1] = vbase + vindex;
            } else {
                uint32_t lvindex = (sindex / tcount) * tcount;
                uint32_t tindex = sindex % tcount;
                dst[0] = sbase + lvindex;
                dst[1] = tbase + tindex;
            }
            return 2;
        }

        char32_t hangul_composition(char32_t u1, char32_t u2) noexcept {
            auto type1 = hangul_syllable_type(u1), type2 = hangul_syllable_type(u2);
            if (type1 == Hangul_Syllable_Type::L && type2 == Hangul_Syllable_Type::V) {
                uint32_t lindex = u1 - lbase;
                uint32_t vindex = u2 - vbase;
                uint32_t lvindex = lindex * ncount + vindex * tcount;
                return sbase + lvindex;
            } else if (type1 == Hangul_Syllable_Type::LV && type2 == Hangul_Syllable_Type::T) {
                char32_t lv[2];
                hangul_decomposition(u1, lv);
                uint32_t lindex = lv[0] - lbase;
                uint32_t vindex = lv[1] - vbase;
                uint32_t tindex = u2 - tbase;
                uint32_t lvindex = lindex * ncount + vindex * tcount;
                return sbase + lvindex + tindex;
            } else {
                return 0;
            }
        }

    }

    // General category

    namespace {

        using CategoryTable = vector<std::pair<uint16_t, uint16_t>>;

        std::function<bool(char32_t)> make_category_function(const CategoryTable& table) {
            return [=] (char32_t c) {
                auto cat = char_general_category(c);
                for (auto& pair: table)
                    if (cat >= pair.first && cat <= pair.second)
                        return true;
                return false;
            };
        }

        CategoryTable make_category_table(const char* cat, size_t n) {
            CategoryTable table;
            static constexpr auto L0 = encode_gc('L', 0);
            if (! cat)
                return table;
            int entries = 0;
            uint16_t prefix = 0;
            for (size_t i = 0; i < n; ++i) {
                char c = cat[i];
                if (ascii_isalpha(c)) {
                    if (prefix == 0) {
                        prefix = encode_gc(ascii_toupper(c), 0);
                    } else if ((c == 'C' || c == 'c') && prefix == L0) {
                        table.push_back({GC::Ll, GC::Ll});
                        table.push_back({GC::Lt, GC::Lu});
                        entries += 2;
                    } else {
                        uint16_t code = prefix + uint8_t(ascii_tolower(c));
                        table.push_back({code, code});
                        ++entries;
                    }
                } else if (c == '&') {
                    if (prefix == L0) {
                        table.push_back({GC::Ll, GC::Ll});
                        table.push_back({GC::Lt, GC::Lu});
                        entries += 2;
                    }
                } else {
                    if (entries == 0 && prefix != 0)
                        table.push_back({prefix, prefix + 0xff});
                    entries = 0;
                    prefix = 0;
                }
            }
            if (entries == 0 && prefix != 0)
                table.push_back({prefix, prefix + 0xff});
            return table;
        }

    }

    const char* gc_name(uint16_t cat) noexcept {
        switch (cat) {
            case GC::Cc:  return "control";
            case GC::Cf:  return "format";
            case GC::Cn:  return "unassigned";
            case GC::Co:  return "private use";
            case GC::Cs:  return "surrogate";
            case GC::Ll:  return "lowercase letter";
            case GC::Lm:  return "modifier letter";
            case GC::Lo:  return "other letter";
            case GC::Lt:  return "titlecase letter";
            case GC::Lu:  return "uppercase letter";
            case GC::Mc:  return "spacing mark";
            case GC::Me:  return "enclosing mark";
            case GC::Mn:  return "nonspacing mark";
            case GC::Nd:  return "decimal number";
            case GC::Nl:  return "letter number";
            case GC::No:  return "other number";
            case GC::Pc:  return "connector punctuation";
            case GC::Pd:  return "dash punctuation";
            case GC::Pe:  return "close punctuation";
            case GC::Pf:  return "final punctuation";
            case GC::Pi:  return "initial punctuation";
            case GC::Po:  return "other punctuation";
            case GC::Ps:  return "open punctuation";
            case GC::Sc:  return "currency symbol";
            case GC::Sk:  return "modifier symbol";
            case GC::Sm:  return "math symbol";
            case GC::So:  return "other symbol";
            case GC::Zl:  return "line separator";
            case GC::Zp:  return "paragraph separator";
            case GC::Zs:  return "space separator";
            default:      return "";
        }
    }

    uint16_t char_general_category(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::general_category_table, c);
    }

    std::function<bool(char32_t)> gc_predicate(uint16_t cat) {
        CategoryTable table;
        table.push_back({cat, cat});
        return make_category_function(table);
    }

    std::function<bool(char32_t)> gc_predicate(const u8string& cat) {
        auto table = make_category_table(cat.data(), cat.size());
        return make_category_function(table);
    }

    std::function<bool(char32_t)> gc_predicate(const char* cat) {
        auto table = make_category_table(cat, strlen(cat));
        return make_category_function(table);
    }

    // Boolean properties

    bool char_is_default_ignorable(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::default_ignorable_table, c); }
    bool char_is_soft_dotted(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::soft_dotted_table, c); }
    bool char_is_white_space(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::white_space_table, c); }
    bool char_is_id_start(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::id_start_table, c); }
    bool char_is_id_nonstart(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::id_nonstart_table, c); }
    bool char_is_xid_start(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::xid_start_table, c); }
    bool char_is_xid_nonstart(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::xid_nonstart_table, c); }
    bool char_is_pattern_syntax(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::pattern_syntax_table, c); }
    bool char_is_pattern_white_space(char32_t c) noexcept { return sparse_set_lookup(UnicornDetail::pattern_white_space_table, c); }

    // Bidirectional properties

    Bidi_Class bidi_class(char32_t c) noexcept {
        using namespace UnicornDetail;
        auto rc = sparse_table_lookup(bidi_class_table, c);
        if (rc != Bidi_Class::Default)
            return rc;
        else if ((c >= 0x600 && c <= 0x7bf)
                || (c >= 0x8a0 && c <= 0x8ff)
                || (c >= 0xfb50 && c <= 0xfdcf)
                || (c >= 0xfdf0 && c <= 0xfdff)
                || (c >= 0xfe70 && c <= 0xfeff)
                || (c >= 0x1ee00 && c <= 0x1eeff))
            return Bidi_Class::AL;
        else if ((c >= 0x590 && c <= 0x5ff)
                || (c >= 0x7c0 && c <= 0x89f)
                || (c >= 0xfb1d && c <= 0xfb4f)
                || (c >= 0x10800 && c <= 0x10fff)
                || (c >= 0x1e800 && c <= 0x1edff)
                || (c >= 0x1ef00 && c <= 0x1efff))
            return Bidi_Class::R;
        else if (c >= 0x20a0 && c <= 0x20cf)
            return Bidi_Class::ET;
        else if (char_is_default_ignorable(c) || char_is_noncharacter(c))
            return Bidi_Class::BN;
        else
            return Bidi_Class::L;
    }

    bool char_is_bidi_mirrored(char32_t c) noexcept {
        return std::binary_search(std::begin(UnicornDetail::bidi_mirrored_table), std::end(UnicornDetail::bidi_mirrored_table), c);
    }

    char32_t bidi_mirroring_glyph(char32_t c) noexcept {
        return table_lookup(UnicornDetail::bidi_mirroring_glyph_table, c, 0);
    }

    char32_t bidi_paired_bracket(char32_t c) noexcept {
        return table_lookup(UnicornDetail::bidi_paired_bracket_table, c, 0);
    }

    char bidi_paired_bracket_type(char32_t c) noexcept {
        return char(table_lookup(UnicornDetail::bidi_paired_bracket_type_table, c, 'n'));
    }

    // Block properties

    namespace {

        class BlockList:
        public vector<BlockInfo> {
        public:
            BlockList() {
                for (auto& kv: UnicornDetail::blocks_table) {
                    if (! empty() && back().last == 0)
                        back().last = kv.key - 1;
                    if (kv.value)
                        push_back({kv.value, kv.key, 0});
                }
            }
        };

    }

    u8string char_block(char32_t c) {
        return cstr(sparse_table_lookup(UnicornDetail::blocks_table, c));
    }

    const vector<BlockInfo>& unicode_block_list() {
        static const BlockList blocks;
        return blocks;
    }

    // Case folding properties

    bool char_is_uppercase(char32_t c) noexcept {
        return sparse_set_lookup(UnicornDetail::other_uppercase_table, c) || char_general_category(c) == GC::Lu;
    }

    bool char_is_lowercase(char32_t c) noexcept {
        return sparse_set_lookup(UnicornDetail::other_lowercase_table, c) || char_general_category(c) == GC::Ll;
    }

    bool char_is_cased(char32_t c) noexcept {
        if (sparse_set_lookup(UnicornDetail::other_uppercase_table, c)
                || sparse_set_lookup(UnicornDetail::other_lowercase_table, c))
            return true;
        auto gc = char_general_category(c);
        return gc == GC::Ll || gc == GC::Lt || gc == GC::Lu;
    }

    bool char_is_case_ignorable(char32_t c) noexcept {
        auto wb = word_break(c);
        if (wb == Word_Break::MidLetter
                || wb == Word_Break::MidNumLet
                || wb == Word_Break::Single_Quote)
            return true;
        auto gc = char_general_category(c);
        return gc == GC::Cf || gc == GC::Lm || gc == GC::Me || gc == GC::Mn || gc == GC::Sk;
    }

    char32_t char_to_simple_uppercase(char32_t c) noexcept {
        return table_lookup(UnicornDetail::simple_uppercase_table, c, c);
    }

    char32_t char_to_simple_lowercase(char32_t c) noexcept {
        return table_lookup(UnicornDetail::simple_lowercase_table, c, c);
    }

    char32_t char_to_simple_titlecase(char32_t c) noexcept {
        using namespace UnicornDetail;
        auto t = table_lookup(simple_titlecase_table, c, not_found);
        return t == not_found ? char_to_simple_uppercase(c) : t;
    }

    char32_t char_to_simple_casefold(char32_t c) noexcept {
        using namespace UnicornDetail;
        auto t = table_lookup(simple_casefold_table, c, not_found);
        return t == not_found ? char_to_simple_lowercase(c) : t;
    }

    size_t char_to_full_uppercase(char32_t c, char32_t* dst) noexcept {
        return extended_table_lookup(c, dst, UnicornDetail::full_uppercase_table, char_to_simple_uppercase);
    }

    size_t char_to_full_lowercase(char32_t c, char32_t* dst) noexcept {
        return extended_table_lookup(c, dst, UnicornDetail::full_lowercase_table, char_to_simple_lowercase);
    }

    size_t char_to_full_titlecase(char32_t c, char32_t* dst) noexcept {
        return extended_table_lookup(c, dst, UnicornDetail::full_titlecase_table, char_to_simple_titlecase);
    }

    size_t char_to_full_casefold(char32_t c, char32_t* dst) noexcept {
        return extended_table_lookup(c, dst, UnicornDetail::full_casefold_table, char_to_simple_casefold);
    }

    // Character names

    namespace {

        class ZlibError:
        public InitializationError {
        public:
            ZlibError(int error):
                InitializationError("Zlib error: " + decode(error)) {}
        private:
            static u8string decode(int error) {
                switch (error) {
                    case Z_ERRNO:          return "Z_ERRNO";
                    case Z_STREAM_ERROR:   return "Z_STREAM_ERROR";
                    case Z_DATA_ERROR:     return "Z_DATA_ERROR";
                    case Z_MEM_ERROR:      return "Z_MEM_ERROR";
                    case Z_BUF_ERROR:      return "Z_BUF_ERROR";
                    case Z_VERSION_ERROR:  return "Z_VERSION_ERROR";
                    default:               return dec(error);
                }
            }
        };

        class CharacterNameMap {
        public:
            CharacterNameMap();
            const u8string& operator[](char32_t c) const;
        private:
            std::unordered_map<char32_t, u8string> map;
        };

        CharacterNameMap::CharacterNameMap() {
            using namespace UnicornDetail;
            u8string names_list(main_names_expanded, 0);
            auto src = reinterpret_cast<const uint8_t*>(main_names_data);
            auto dst = reinterpret_cast<uint8_t*>(&names_list[0]);
            unsigned long dstlen = main_names_expanded;
            auto rc = uncompress(dst, &dstlen, src, main_names_compressed);
            if (rc != Z_OK)
                throw ZlibError(rc);
            size_t i = 0;
            while (i < main_names_expanded) {
                auto j = names_list.find(';', i);
                auto k = names_list.find(';', j + 1);
                if (j == npos || k == npos)
                    break;
                auto c = char32_t(strtoul(names_list.data() + i, nullptr, 16));
                map[c] = names_list.substr(j + 1, k - j - 1);
                i = k + 1;
            }
        }

        const u8string& CharacterNameMap::operator[](char32_t c) const {
            static const u8string dummy;
            auto i = map.find(c);
            return i == map.end() ? dummy : i->second;
        }

        bool is_unified_ideograph(char32_t c) noexcept {
            return (c >= 0x3400 && c <= 0x4dbf) || (c >= 0x4e00 && c <= 0x9fff)
                || (c >= 0x20000 && c <= 0x2a6df) || (c >= 0x2a700 && c <= 0x2b81f);
        }

        bool is_compatibility_ideograph(char32_t c) noexcept {
            return (c >= 0xf900 && c <= 0xfaff) || (c >= 0x2f800 && c <= 0x2fa1f);
        }

        u8string hangul_name(char32_t c) {
            // Based on code in section 3.12 of the Unicode Standard
            static constexpr uint32_t s_base = 0xac00,
                l_count = 19, v_count = 21, t_count = 28,
                n_count = v_count * t_count, s_count = l_count * n_count;
            static constexpr const char* jamo_l_table[] {
                "G", "GG", "N", "D", "DD", "R", "M", "B", "BB",
                "S", "SS", "", "J", "JJ", "C", "K", "T", "P", "H"
            };
            static constexpr const char* jamo_v_table[] {
                "A", "AE", "YA", "YAE", "EO", "E", "YEO", "YE", "O",
                "WA", "WAE", "OE", "YO", "U", "WEO", "WE", "WI",
                "YU", "EU", "YI", "I"
            };
            static constexpr const char* jamo_t_table[] {
                "", "G", "GG", "GS", "N", "NJ", "NH", "D", "L", "LG", "LM",
                "LB", "LS", "LT", "LP", "LH", "M", "B", "BS",
                "S", "SS", "NG", "J", "C", "K", "T", "P", "H"
            };
            if (c < s_base || c - s_base >= s_count)
                return {};
            auto s_index = c - s_base;
            auto l_index = s_index / n_count;
            auto v_index = (s_index % n_count) / t_count;
            auto t_index = s_index % t_count;
            return "HANGUL SYLLABLE "s + jamo_l_table[l_index]
                + jamo_v_table[v_index] + jamo_t_table[t_index];
        }

        const char* control_character_name(char32_t c) {
            switch (c) {
                case 0x00: return "NULL";
                case 0x01: return "START OF HEADING";
                case 0x02: return "START OF TEXT";
                case 0x03: return "END OF TEXT";
                case 0x04: return "END OF TRANSMISSION";
                case 0x05: return "ENQUIRY";
                case 0x06: return "ACKNOWLEDGE";
                case 0x07: return "ALERT"; // BELL is spoken for (U+1F514)
                case 0x08: return "BACKSPACE";
                case 0x09: return "HORIZONTAL TABULATION";
                case 0x0a: return "LINE FEED";
                case 0x0b: return "VERTICAL TABULATION";
                case 0x0c: return "FORM FEED";
                case 0x0d: return "CARRIAGE RETURN";
                case 0x0e: return "SHIFT OUT";
                case 0x0f: return "SHIFT IN";
                case 0x10: return "DATA LINK ESCAPE";
                case 0x11: return "DEVICE CONTROL 1";
                case 0x12: return "DEVICE CONTROL 2";
                case 0x13: return "DEVICE CONTROL 3";
                case 0x14: return "DEVICE CONTROL 4";
                case 0x15: return "NEGATIVE ACKNOWLEDGE";
                case 0x16: return "SYNCHRONOUS IDLE";
                case 0x17: return "END OF TRANSMISSION BLOCK";
                case 0x18: return "CANCEL";
                case 0x19: return "END OF MEDIUM";
                case 0x1a: return "SUBSTITUTE";
                case 0x1b: return "ESCAPE";
                case 0x1c: return "FIELD SEPARATOR";
                case 0x1d: return "GROUP SEPARATOR";
                case 0x1e: return "RECORD SEPARATOR";
                case 0x1f: return "UNIT SEPARATOR";
                case 0x7f: return "DELETE";
                case 0x80: return "PADDING CHARACTER";
                case 0x81: return "HIGH OCTET PRESET";
                case 0x82: return "BREAK PERMITTED HERE";
                case 0x83: return "NO BREAK HERE";
                case 0x84: return "INDEX";
                case 0x85: return "NEXT LINE";
                case 0x86: return "START OF SELECTED AREA";
                case 0x87: return "END OF SELECTED AREA";
                case 0x88: return "CHARACTER TABULATION SET";
                case 0x89: return "CHARACTER TABULATION WITH JUSTIFICATION";
                case 0x8a: return "LINE TABULATION SET";
                case 0x8b: return "PARTIAL LINE FORWARD";
                case 0x8c: return "PARTIAL LINE BACKWARD";
                case 0x8d: return "REVERSE LINE FEED";
                case 0x8e: return "SINGLE SHIFT 2";
                case 0x8f: return "SINGLE SHIFT 3";
                case 0x90: return "DEVICE CONTROL STRING";
                case 0x91: return "PRIVATE USE 1";
                case 0x92: return "PRIVATE USE 2";
                case 0x93: return "SET TRANSMIT STATE";
                case 0x94: return "CANCEL CHARACTER";
                case 0x95: return "MESSAGE WAITING";
                case 0x96: return "START OF GUARDED AREA";
                case 0x97: return "END OF GUARDED AREA";
                case 0x98: return "START OF STRING";
                case 0x99: return "SINGLE GRAPHIC CHARACTER INTRODUCER";
                case 0x9a: return "SINGLE CHARACTER INTRODUCER";
                case 0x9b: return "CONTROL SEQUENCE INTRODUCER";
                case 0x9c: return "STRING TERMINATOR";
                case 0x9d: return "OPERATING SYSTEM COMMAND";
                case 0x9e: return "PRIVACY MESSAGE";
                case 0x9f: return "APPLICATION PROGRAM COMMAND";
                default: return nullptr;
            }
        }

    }

    u8string char_name(char32_t c, uint32_t flags) {
        using namespace UnicornDetail;
        static const CharacterNameMap map;
        u8string name;
        if (flags & cn_control) {
            auto name_ptr = control_character_name(c);
            if (name_ptr)
                name = name_ptr;
        }
        if (name.empty() && (flags & cn_update)) {
            auto name_ptr = table_lookup(corrected_names_table, c, static_cast<const char*>(nullptr));
            if (name_ptr)
                name = name_ptr;
        }
        if (name.empty())
            name = map[c];
        if (name.empty()) {
            if (is_unified_ideograph(c))
                name = "CJK UNIFIED IDEOGRAPH-" + ascii_uppercase(hex(c, 4));
            else if (is_compatibility_ideograph(c))
                name = "CJK COMPATIBILITY IDEOGRAPH-" + ascii_uppercase(hex(c, 4));
            else
                name = hangul_name(c);
        }
        if (flags & cn_lower)
            name = ascii_lowercase(name);
        if (name.empty() && (flags & cn_label)) {
            if (c <= last_unicode_char) {
                auto gc = char_general_category(c);
                if (gc == GC::Cc)
                    name = "<control-";
                else if (gc == GC::Co)
                    name = "<private-use-";
                else if (gc == GC::Cs)
                    name = "<surrogate-";
                else if (char_is_noncharacter(c))
                    name = "<noncharacter-";
                else
                    name = "<reserved-";
            } else {
                name = "<noncharacter-";
            }
            if (flags & cn_lower)
                name += hex(c, 4);
            else
                name += ascii_uppercase(hex(c, 4));
            name += '>';
        }
        if (flags & cn_prefix) {
            u8string prefix = char_as_hex(c);
            if (name.empty())
                name = prefix;
            else
                name = prefix + ' ' + name;
        }
        return name;
    }

    // Decomposition properties

    int combining_class(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::combining_class_table, c);
    }

    char32_t canonical_composition(char32_t u1, char32_t u2) noexcept {
        using namespace UnicornDetail;
        char32_t c(hangul_composition(u1, u2));
        if (! c) {
            std::array<char32_t, 2> key{{u1, u2}};
            c = table_lookup(composition_table, key, char32_t(0));
        }
        return c;
    }

    size_t canonical_decomposition(char32_t c, char32_t* dst) noexcept {
        using namespace UnicornDetail;
        size_t n(hangul_decomposition(c, dst));
        if (! n)
            n = extended_table_lookup(c, dst, canonical_table);
        return n;
    }

    size_t compatibility_decomposition(char32_t c, char32_t* dst) noexcept {
        using namespace UnicornDetail;
        size_t n(canonical_decomposition(c, dst));
        if (! n)
            n = extended_table_lookup(c, dst, short_compatibility_table);
        if (! n)
            n = extended_table_lookup(c, dst, long_compatibility_table);
        return n;
    }

    // Enumeration properties

    East_Asian_Width east_asian_width(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::east_asian_width_table, c);
    }

    Grapheme_Cluster_Break grapheme_cluster_break(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::grapheme_cluster_break_table, c);
    }

    Hangul_Syllable_Type hangul_syllable_type(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::hangul_syllable_type_table, c);
    }

    Indic_Positional_Category indic_positional_category(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::indic_positional_category_table, c);
    }

    Indic_Syllabic_Category indic_syllabic_category(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::indic_syllabic_category_table, c);
    }

    Joining_Group joining_group(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::joining_group_table, c);
    }

    Joining_Type joining_type(char32_t c) noexcept {
        auto rc = sparse_table_lookup(UnicornDetail::joining_type_table, c);
        if (rc != Joining_Type::Default)
            return rc;
        auto gc = char_general_category(c);
        if (gc == GC::Cf || gc == GC::Me || gc == GC::Mn)
            return Joining_Type::Transparent;
        else
            return Joining_Type::Non_Joining;
    }

    Line_Break line_break(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::line_break_table, c);
    }

    Numeric_Type numeric_type(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::numeric_type_table, c);
    }

    Sentence_Break sentence_break(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::sentence_break_table, c);
    }

    Word_Break word_break(char32_t c) noexcept {
        return sparse_table_lookup(UnicornDetail::word_break_table, c);
    }

    // The casts to int in the property_value() functions are to work around a
    // bug in Clang's unreachable code warning
    // (http://llvm.org/bugs/show_bug.cgi?id=10444)

    #define PROPVAL(val) case int(enum_type::val): return # val

    u8string property_value(Bidi_Class val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(AL);
            PROPVAL(AN);
            PROPVAL(B);
            PROPVAL(BN);
            PROPVAL(CS);
            PROPVAL(EN);
            PROPVAL(ES);
            PROPVAL(ET);
            PROPVAL(FSI);
            PROPVAL(L);
            PROPVAL(LRE);
            PROPVAL(LRI);
            PROPVAL(LRO);
            PROPVAL(NSM);
            PROPVAL(ON);
            PROPVAL(PDF);
            PROPVAL(PDI);
            PROPVAL(R);
            PROPVAL(RLE);
            PROPVAL(RLI);
            PROPVAL(RLO);
            PROPVAL(S);
            PROPVAL(WS);
            default: return dec(x);
        }
    }

    u8string property_value(East_Asian_Width val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(N);
            PROPVAL(A);
            PROPVAL(F);
            PROPVAL(H);
            PROPVAL(Na);
            PROPVAL(W);
            default: return dec(x);
        }
    }

    u8string property_value(Grapheme_Cluster_Break val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(Other);
            PROPVAL(Control);
            PROPVAL(CR);
            PROPVAL(EOT);
            PROPVAL(Extend);
            PROPVAL(L);
            PROPVAL(LF);
            PROPVAL(LV);
            PROPVAL(LVT);
            PROPVAL(Prepend);
            PROPVAL(Regional_Indicator);
            PROPVAL(SOT);
            PROPVAL(SpacingMark);
            PROPVAL(T);
            PROPVAL(V);
            default: return dec(x);
        }
    }

    u8string property_value(Hangul_Syllable_Type val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(NA);
            PROPVAL(L);
            PROPVAL(LV);
            PROPVAL(LVT);
            PROPVAL(T);
            PROPVAL(V);
            default: return dec(x);
        }
    }

    u8string property_value(Indic_Positional_Category val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(NA);
            PROPVAL(Bottom);
            PROPVAL(Bottom_And_Right);
            PROPVAL(Left);
            PROPVAL(Left_And_Right);
            PROPVAL(Overstruck);
            PROPVAL(Right);
            PROPVAL(Top);
            PROPVAL(Top_And_Bottom);
            PROPVAL(Top_And_Bottom_And_Right);
            PROPVAL(Top_And_Left);
            PROPVAL(Top_And_Left_And_Right);
            PROPVAL(Top_And_Right);
            PROPVAL(Visual_Order_Left);
            default: return dec(x);
        }
    }

    u8string property_value(Indic_Syllabic_Category val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(Other);
            PROPVAL(Avagraha);
            PROPVAL(Bindu);
            PROPVAL(Brahmi_Joining_Number);
            PROPVAL(Cantillation_Mark);
            PROPVAL(Consonant);
            PROPVAL(Consonant_Dead);
            PROPVAL(Consonant_Final);
            PROPVAL(Consonant_Head_Letter);
            PROPVAL(Consonant_Killer);
            PROPVAL(Consonant_Medial);
            PROPVAL(Consonant_Placeholder);
            PROPVAL(Consonant_Preceding_Repha);
            PROPVAL(Consonant_Prefixed);
            PROPVAL(Consonant_Subjoined);
            PROPVAL(Consonant_Succeeding_Repha);
            PROPVAL(Consonant_With_Stacker);
            PROPVAL(Gemination_Mark);
            PROPVAL(Invisible_Stacker);
            PROPVAL(Joiner);
            PROPVAL(Modifying_Letter);
            PROPVAL(Non_Joiner);
            PROPVAL(Nukta);
            PROPVAL(Number);
            PROPVAL(Number_Joiner);
            PROPVAL(Pure_Killer);
            PROPVAL(Register_Shifter);
            PROPVAL(Syllable_Modifier);
            PROPVAL(Tone_Letter);
            PROPVAL(Tone_Mark);
            PROPVAL(Virama);
            PROPVAL(Visarga);
            PROPVAL(Vowel);
            PROPVAL(Vowel_Dependent);
            PROPVAL(Vowel_Independent);
            default: return dec(x);
        }
    }

    u8string property_value(Joining_Group val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(No_Joining_Group);
            PROPVAL(Ain);
            PROPVAL(Alaph);
            PROPVAL(Alef);
            PROPVAL(Beh);
            PROPVAL(Beth);
            PROPVAL(Burushaski_Yeh_Barree);
            PROPVAL(Dal);
            PROPVAL(Dalath_Rish);
            PROPVAL(E);
            PROPVAL(Farsi_Yeh);
            PROPVAL(Fe);
            PROPVAL(Feh);
            PROPVAL(Final_Semkath);
            PROPVAL(Gaf);
            PROPVAL(Gamal);
            PROPVAL(Hah);
            PROPVAL(He);
            PROPVAL(Heh);
            PROPVAL(Heh_Goal);
            PROPVAL(Heth);
            PROPVAL(Kaf);
            PROPVAL(Kaph);
            PROPVAL(Khaph);
            PROPVAL(Knotted_Heh);
            PROPVAL(Lam);
            PROPVAL(Lamadh);
            PROPVAL(Manichaean_Aleph);
            PROPVAL(Manichaean_Ayin);
            PROPVAL(Manichaean_Beth);
            PROPVAL(Manichaean_Daleth);
            PROPVAL(Manichaean_Dhamedh);
            PROPVAL(Manichaean_Five);
            PROPVAL(Manichaean_Gimel);
            PROPVAL(Manichaean_Heth);
            PROPVAL(Manichaean_Hundred);
            PROPVAL(Manichaean_Kaph);
            PROPVAL(Manichaean_Lamedh);
            PROPVAL(Manichaean_Mem);
            PROPVAL(Manichaean_Nun);
            PROPVAL(Manichaean_One);
            PROPVAL(Manichaean_Pe);
            PROPVAL(Manichaean_Qoph);
            PROPVAL(Manichaean_Resh);
            PROPVAL(Manichaean_Sadhe);
            PROPVAL(Manichaean_Samekh);
            PROPVAL(Manichaean_Taw);
            PROPVAL(Manichaean_Ten);
            PROPVAL(Manichaean_Teth);
            PROPVAL(Manichaean_Thamedh);
            PROPVAL(Manichaean_Twenty);
            PROPVAL(Manichaean_Waw);
            PROPVAL(Manichaean_Yodh);
            PROPVAL(Manichaean_Zayin);
            PROPVAL(Meem);
            PROPVAL(Mim);
            PROPVAL(Noon);
            PROPVAL(Nun);
            PROPVAL(Nya);
            PROPVAL(Pe);
            PROPVAL(Qaf);
            PROPVAL(Qaph);
            PROPVAL(Reh);
            PROPVAL(Reversed_Pe);
            PROPVAL(Rohingya_Yeh);
            PROPVAL(Sad);
            PROPVAL(Sadhe);
            PROPVAL(Seen);
            PROPVAL(Semkath);
            PROPVAL(Shin);
            PROPVAL(Straight_Waw);
            PROPVAL(Swash_Kaf);
            PROPVAL(Syriac_Waw);
            PROPVAL(Tah);
            PROPVAL(Taw);
            PROPVAL(Teh_Marbuta);
            PROPVAL(Teh_Marbuta_Goal);
            PROPVAL(Teth);
            PROPVAL(Waw);
            PROPVAL(Yeh);
            PROPVAL(Yeh_Barree);
            PROPVAL(Yeh_With_Tail);
            PROPVAL(Yudh);
            PROPVAL(Yudh_He);
            PROPVAL(Zain);
            PROPVAL(Zhain);
            default: return dec(x);
        }
    }

    u8string property_value(Joining_Type val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(Dual_Joining);
            PROPVAL(Join_Causing);
            PROPVAL(Left_Joining);
            PROPVAL(Non_Joining);
            PROPVAL(Right_Joining);
            PROPVAL(Transparent);
            default: return dec(x);
        }
    }

    u8string property_value(Line_Break val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(XX);
            PROPVAL(AI);
            PROPVAL(AL);
            PROPVAL(B2);
            PROPVAL(BA);
            PROPVAL(BB);
            PROPVAL(BK);
            PROPVAL(CB);
            PROPVAL(CJ);
            PROPVAL(CL);
            PROPVAL(CM);
            PROPVAL(CP);
            PROPVAL(CR);
            PROPVAL(EX);
            PROPVAL(GL);
            PROPVAL(H2);
            PROPVAL(H3);
            PROPVAL(HL);
            PROPVAL(HY);
            PROPVAL(ID);
            PROPVAL(IN);
            PROPVAL(IS);
            PROPVAL(JL);
            PROPVAL(JT);
            PROPVAL(JV);
            PROPVAL(LF);
            PROPVAL(NL);
            PROPVAL(NS);
            PROPVAL(NU);
            PROPVAL(OP);
            PROPVAL(PO);
            PROPVAL(PR);
            PROPVAL(QU);
            PROPVAL(RI);
            PROPVAL(SA);
            PROPVAL(SG);
            PROPVAL(SP);
            PROPVAL(SY);
            PROPVAL(WJ);
            PROPVAL(ZW);
            default: return dec(x);
        }
    }

    u8string property_value(Numeric_Type val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(None);
            PROPVAL(Decimal);
            PROPVAL(Digit);
            PROPVAL(Numeric);
            default: return dec(x);
        }
    }

    u8string property_value(Sentence_Break val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(Other);
            PROPVAL(ATerm);
            PROPVAL(Close);
            PROPVAL(CR);
            PROPVAL(EOT);
            PROPVAL(Extend);
            PROPVAL(Format);
            PROPVAL(LF);
            PROPVAL(Lower);
            PROPVAL(Numeric);
            PROPVAL(OLetter);
            PROPVAL(SContinue);
            PROPVAL(Sep);
            PROPVAL(SOT);
            PROPVAL(Sp);
            PROPVAL(STerm);
            PROPVAL(Upper);
            default: return dec(x);
        }
    }

    u8string property_value(Word_Break val) {
        using enum_type = decltype(val);
        auto x = int(val);
        switch (x) {
            PROPVAL(Other);
            PROPVAL(ALetter);
            PROPVAL(CR);
            PROPVAL(Double_Quote);
            PROPVAL(EOT);
            PROPVAL(Extend);
            PROPVAL(ExtendNumLet);
            PROPVAL(Format);
            PROPVAL(Hebrew_Letter);
            PROPVAL(Katakana);
            PROPVAL(LF);
            PROPVAL(MidLetter);
            PROPVAL(MidNum);
            PROPVAL(MidNumLet);
            PROPVAL(Newline);
            PROPVAL(Numeric);
            PROPVAL(Regional_Indicator);
            PROPVAL(Single_Quote);
            PROPVAL(SOT);
            default: return dec(x);
        }
    }

    // Numeric properties

    std::pair<long long, long long> numeric_value(char32_t c) {
        const auto pair = sparse_table_lookup(UnicornDetail::numeric_value_table, c);
        return {pair.first, pair.second};
    }

    // Script properties

    namespace {

        u8string decode_script(uint32_t code) {
            u8string s;
            for (int n = 24; n >= 0; n -= 8)
                s += char((code >> n) & 0xff);
            s[0] = ascii_toupper(s[0]);
            return s;
        }

        uint32_t encode_script(u8string abbr) {
            abbr.resize(4, 0);
            uint32_t code = 0;
            for (char c: abbr)
                code = (code << 8) | uint8_t(ascii_tolower(c));
            return code;
        }

        class ScriptNameMap {
        public:
            ScriptNameMap();
            u8string operator[](const u8string& abbr) const;
        private:
            std::unordered_map<uint32_t, u8string> map;
        };

        ScriptNameMap::ScriptNameMap() {
            for (auto& info: UnicornDetail::iso_script_names)
                map.insert({encode_script(info.abbr), u8string(info.name)});
        }

        u8string ScriptNameMap::operator[](const u8string& abbr) const {
            auto i = map.find(encode_script(abbr));
            return i == map.end() ? ""s : i->second;
        }

        class ScriptExtensionMap {
        public:
            ScriptExtensionMap();
            vector<u8string> operator[](const char* cp) const;
        private:
            std::unordered_map<const char*, vector<u8string>> map;
        };

        ScriptExtensionMap::ScriptExtensionMap() {
            for (auto& kv: UnicornDetail::script_extensions_table) {
                if (kv.value) {
                    vector<u8string> scripts;
                    u8string list(kv.value);
                    for (size_t i = 0; i < list.size(); i += 5)
                        scripts.push_back(list.substr(i, 4));
                    map[kv.value] = scripts;
                }
            }
        }

        vector<u8string> ScriptExtensionMap::operator[](const char* cp) const {
            auto i = map.find(cp);
            if (i == map.end())
                return {};
            else
                return i->second;
        }

    }

    u8string char_script(char32_t c) {
        return decode_script(sparse_table_lookup(UnicornDetail::scripts_table, c));
    }

    vector<u8string> char_script_list(char32_t c) {
        static const ScriptExtensionMap map;
        auto cp = sparse_table_lookup(UnicornDetail::script_extensions_table, c);
        if (cp)
            return map[cp];
        else
            return {char_script(c)};
    }

    u8string script_name(const u8string& abbr) {
        static const ScriptNameMap map;
        return map[abbr];
    }

}
