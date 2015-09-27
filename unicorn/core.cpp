#include "unicorn/character.hpp"
#include "unicorn/ucd-tables.hpp"
#include <algorithm>

using namespace std::literals;

namespace Unicorn {

    namespace UnicornDetail {

        // Flag manipulation functions and macros

        namespace {

            struct FlagInfo {
                u8string context;
                u8string name;
                uint64_t value;
            };

            u8string read_write_flag(char rw, const u8string& context, const u8string& name, uint64_t value) {
                static vector<FlagInfo> table;
                static Mutex mutex;
                MutexLock lock(mutex);
                if (rw == 'w') {
                    table.push_back({context, name, value});
                    return name;
                } else {
                    auto it = table.begin(), end = table.end(), backup = end;
                    for (; it != end; ++it) {
                        if (it->value == value) {
                            if (it->context == context)
                                return it->name;
                            if (backup == end)
                                backup = it;
                        }
                    }
                    if (backup == end)
                        return {};
                    else
                        return backup->name;
                }
            }

        }

        RecordFlag::RecordFlag(const u8string& context, const  u8string& name, uint64_t value) {
            read_write_flag('w', context, name, value);
        }

        u8string flag_name(const u8string& context, uint64_t flag) {
            return read_write_flag('r', context, "", flag);
        }

        u8string flag_names(const u8string& context, uint64_t flags) {
            vector<u8string> names;
            while (flags) {
                uint64_t f = 1ull << __builtin_ctzll(flags);
                names.push_back(flag_name(context, f));
                flags &= ~ f;
            }
            return join_words(names, ", ");
        }

        void allow_flags(uint64_t flags, uint64_t allow, const u8string& context) {
            if (flags & ~ allow)
                throw std::invalid_argument("Invalid " + context + " flags: 0x" + hex(flags, 1));
        }

        void exclusive_flags(uint64_t flags, uint64_t exclusive, const u8string& context) {
            if (__builtin_popcountll(flags & exclusive) > 1)
                throw std::invalid_argument("Inconsistent " + context + " flags: " + flag_names(context, flags & exclusive));
        }

    }

    // Exceptions

    u8string EncodingError::prefix(const u8string& encoding, size_t offset) {
        u8string s = "Encoding error";
        if (! encoding.empty()) {
            s += " (";
            s += encoding;
            s += ")";
        }
        if (offset > 0) {
            s += "; offset ";
            s += dec(offset);
        }
        return s;
    }

    // Basic character types

    namespace Literals {

        NativeString operator"" _nat(const char* s, size_t n) {
            NativeString ns(n, '\0');
            std::transform(s, s + n, ns.begin(), [] (char c) { return NativeCharacter(uint8_t(c)); });
            return ns;
        }

    }

    // Version information

    namespace UnicornDetail {

        UnicodeVersionTable::UnicodeVersionTable() {
            table[{1,1,0}] = 0x00a0;  // Unicode 1.1 (Jun 1993): U+00A0 no-break space
            table[{2,0,0}] = 0x0591;  // Unicode 2.0 (Jul 1996): U+0591 hebrew accent etnahta
            table[{2,1,0}] = 0x20ac;  // Unicode 2.1 (May 1998): U+20AC euro sign
            table[{3,0,0}] = 0x01f6;  // Unicode 3.0 (Sep 1999): U+01F6 latin capital letter hwair
            table[{3,1,0}] = 0x03f4;  // Unicode 3.1 (Mar 2001): U+03F4 greek capital theta symbol
            table[{3,2,0}] = 0x0220;  // Unicode 3.2 (Mar 2002): U+0220 latin capital letter n with long right leg
            table[{4,0,0}] = 0x0221;  // Unicode 4.0 (Apr 2003): U+0221 latin small letter d with curl
            table[{4,1,0}] = 0x0237;  // Unicode 4.1 (Mar 2005): U+0237 latin small letter dotless j
            table[{5,0,0}] = 0x0242;  // Unicode 5.0 (Jul 2006): U+0242 latin small letter glottal stop
            table[{5,1,0}] = 0x0370;  // Unicode 5.1 (Mar 2008): U+0370 greek capital letter heta
            table[{5,2,0}] = 0x0524;  // Unicode 5.2 (Oct 2009): U+0524 cyrillic capital letter pe with descender
            table[{6,0,0}] = 0x0526;  // Unicode 6.0 (Sep 2010): U+0526 cyrillic capital letter shha with descender
            table[{6,1,0}] = 0x058f;  // Unicode 6.1 (Jan 2012): U+058F armenian dram sign
            table[{6,2,0}] = 0x20ba;  // Unicode 6.2 (Sep 2012): U+20BA turkish lira sign
            table[{6,3,0}] = 0x061c;  // Unicode 6.3 (Sep 2013): U+061C arabic letter mark
            table[{7,0,0}] = 0x037f;  // Unicode 7.0 (Jun 2014): U+037F greek capital letter yot
            table[{8,0,0}] = 0x08b3;  // Unicode 8.0 (Jun 2015): U+08B3 arabic letter ain with three dots below
        }

        const UnicodeVersionTable& unicode_version_table() {
            static const UnicodeVersionTable uvt;
            return uvt;
        }

    }

    namespace {

        Version check_unicode_version() noexcept {
            Version v {0,0,0};
            auto& table = UnicornDetail::unicode_version_table().table;
            for (auto& entry: table) {
                if (sparse_table_lookup(UnicornDetail::general_category_table, entry.second) == 0x436e) // Cn
                    break;
                v = entry.first;
            }
            return v;
        }

    }

    Version unicorn_version() noexcept {
        return {0,1,0};
    }

    Version unicode_version() noexcept {
        static const Version v = check_unicode_version();
        return v;
    }

}
