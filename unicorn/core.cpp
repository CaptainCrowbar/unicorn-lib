#include "unicorn/character.hpp"
#include "unicorn/ucd-tables.hpp"

using namespace std::literals;

namespace Unicorn {

    namespace {

        static constexpr Crow::Version current_version {0,1,0};

        struct UnicodeVersion {
            char32_t code;
            Crow::Version version;
        };

        const UnicodeVersion unicode_versions[] {
            { 0x00a0, {1,1,0} },  // Unicode 1.1 (Jun 1993): U+00A0 no-break space
            { 0x0591, {2,0,0} },  // Unicode 2.0 (Jul 1996): U+0591 hebrew accent etnahta
            { 0x20ac, {2,1,0} },  // Unicode 2.1 (May 1998): U+20AC euro sign
            { 0x01f6, {3,0,0} },  // Unicode 3.0 (Sep 1999): U+01F6 latin capital letter hwair
            { 0x03f4, {3,1,0} },  // Unicode 3.1 (Mar 2001): U+03F4 greek capital theta symbol
            { 0x0220, {3,2,0} },  // Unicode 3.2 (Mar 2002): U+0220 latin capital letter n with long right leg
            { 0x0221, {4,0,0} },  // Unicode 4.0 (Apr 2003): U+0221 latin small letter d with curl
            { 0x0237, {4,1,0} },  // Unicode 4.1 (Mar 2005): U+0237 latin small letter dotless j
            { 0x0242, {5,0,0} },  // Unicode 5.0 (Jul 2006): U+0242 latin small letter glottal stop
            { 0x0370, {5,1,0} },  // Unicode 5.1 (Mar 2008): U+0370 greek capital letter heta
            { 0x0524, {5,2,0} },  // Unicode 5.2 (Oct 2009): U+0524 cyrillic capital letter pe with descender
            { 0x0526, {6,0,0} },  // Unicode 6.0 (Sep 2010): U+0526 cyrillic capital letter shha with descender
            { 0x058f, {6,1,0} },  // Unicode 6.1 (Jan 2012): U+058F armenian dram sign
            { 0x20ba, {6,2,0} },  // Unicode 6.2 (Sep 2012): U+20BA turkish lira sign
            { 0x061c, {6,3,0} },  // Unicode 6.3 (Sep 2013): U+061C arabic letter mark
            { 0x037f, {7,0,0} },  // Unicode 7.0 (Jun 2014): U+037F greek capital letter yot
            { 0x08b3, {8,0,0} },  // Unicode 8.0 (Jun 2015): U+08B3 arabic letter ain with three dots below
        };

        Crow::Version check_unicode_version() noexcept {
            Crow::Version v {0,0,0};
            for (auto& uv: unicode_versions) {
                if (sparse_table_lookup(UnicornDetail::general_category_table, uv.code) == 0x436e) // Cn
                    break;
                v = uv.version;
            }
            return v;
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
            s += Crow::dec(offset);
        }
        return s;
    }

    // Version information

    Crow::Version unicorn_version() noexcept {
        return current_version;
    }

    Crow::Version unicode_version() noexcept {
        static const Crow::Version v = check_unicode_version();
        return v;
    }

}
