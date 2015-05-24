#include "unicorn/core.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/unit-test.hpp"
#include <cerrno>
#include <iostream>
#include <string>

#if defined(_WIN32)
    #include <windows.h>
#endif

using namespace std::literals;
using namespace Unicorn;

namespace {

    using UnicornDetail::lookup_encoding;

    const auto euro_utf8         = "€uro"s;
    const auto euro_utf16        = u"€uro"s;
    const auto euro_utf32        = U"€uro"s;
    const auto euro_utf16be      = "\x20\xac\x00\x75\x00\x72\x00\x6f"s;
    const auto euro_utf16le      = "\xac\x20\x75\x00\x72\x00\x6f\x00"s;
    const auto euro_utf32be      = "\x00\x00\x20\xac\x00\x00\x00\x75\x00\x00\x00\x72\x00\x00\x00\x6f"s;
    const auto euro_utf32le      = "\xac\x20\x00\x00\x75\x00\x00\x00\x72\x00\x00\x00\x6f\x00\x00\x00"s;
    const auto euro_windows1252  = "\x80\x75\x72\x6f"s;
    const auto euro_iso8859_15   = "\xa4\x75\x72\x6f"s;
    const auto euro_gb18030      = "\xa2\xe3\x75\x72\x6f"s;

    void check_locale_detection() {

        std::string s;

        u8string loc;
        TRY(loc = local_encoding());
        if (loc.empty())
            loc = "<unknown>";
        else
            TRY(lookup_encoding(loc));
        std::cout << "... Local encoding: " << loc << "\n";

    }

    void check_utf_detection() {

        TEST_EQUAL(UnicornDetail::guess_utf("\x00\x00\xfe\xff"s), "utf-32be");
        TEST_EQUAL(UnicornDetail::guess_utf("\xff\xfe\x00\x00"s), "utf-32le");
        TEST_EQUAL(UnicornDetail::guess_utf("\xfe\xff\x00\x41"s), "utf-16be");
        TEST_EQUAL(UnicornDetail::guess_utf("\xff\xfe\x41\x00"s), "utf-16le");
        TEST_EQUAL(UnicornDetail::guess_utf("\xef\xbb\xbf\x41"s), "utf-8");
        TEST_EQUAL(UnicornDetail::guess_utf("\x00\x00\x00\x41"s), "utf-32be");
        TEST_EQUAL(UnicornDetail::guess_utf("\x00\x00\x20\xac"s), "utf-32be");
        TEST_EQUAL(UnicornDetail::guess_utf("\x41\x00\x00\x00"s), "utf-32le");
        TEST_EQUAL(UnicornDetail::guess_utf("\xac\x20\x00\x00"s), "utf-32le");
        TEST_EQUAL(UnicornDetail::guess_utf("\x00\x41\x00\x42"s), "utf-16be");
        TEST_EQUAL(UnicornDetail::guess_utf("\x41\x00\x42\x00"s), "utf-16le");
        TEST_EQUAL(UnicornDetail::guess_utf("\x41\x42\x43\x44"s), "utf-8");
        TEST_EQUAL(UnicornDetail::guess_utf("\x00\x00\x00\x00"s), "utf-8");
        TEST_EQUAL(UnicornDetail::guess_utf(euro_utf8), "utf-8");
        TEST_EQUAL(UnicornDetail::guess_utf(euro_utf16be), "utf-16be");
        TEST_EQUAL(UnicornDetail::guess_utf(euro_utf16le), "utf-16le");
        TEST_EQUAL(UnicornDetail::guess_utf(euro_utf32be), "utf-32be");
        TEST_EQUAL(UnicornDetail::guess_utf(euro_utf32le), "utf-32le");

    }

    void check_encoding_queries() {

        #if defined(_XOPEN_SOURCE)

            TEST_EQUAL(lookup_encoding(20127), "us-ascii");
            TEST_EQUAL(lookup_encoding("ASCII"), "ascii");
            TEST_EQUAL(lookup_encoding("UTF-8"), "utf-8");
            TEST_EQUAL(lookup_encoding("UTF-16LE"), "utf-16");
            TEST_EQUAL(lookup_encoding("UTF-16BE"), "utf-16-swapped");
            TEST_EQUAL(lookup_encoding("GB18030"), "gb18030");

        #else

            TEST_EQUAL(lookup_encoding(20127), 20127);
            TEST_EQUAL(lookup_encoding("ASCII"), 20127);
            TEST_EQUAL(lookup_encoding("UTF-8"), 65001);
            TEST_EQUAL(lookup_encoding("UTF-16LE"), 1200);
            TEST_EQUAL(lookup_encoding("UTF-16BE"), 1201);
            TEST_EQUAL(lookup_encoding("GB18030"), 54936);

        #endif

        TEST_THROW(lookup_encoding(99999), UnknownEncoding);
        TEST_THROW(lookup_encoding("Voynich"), UnknownEncoding);

    }

    void check_mbcs_to_unicode() {

        u8string s8;
        std::u16string s16;
        std::u32string s32;

        TRY(import_string(euro_utf8, s8, "utf-8"));                TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16be, s8, "utf-16be"));          TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16le, s8, "utf-16le"));          TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32be, s8, "utf-32be"));          TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32le, s8, "utf-32le"));          TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_windows1252, s8, "windows-1252"));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_iso8859_15, s8, "iso-8859-15"));    TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_gb18030, s8, "gb18030"));           TEST_EQUAL(s8, euro_utf8);

        TRY(import_string(euro_utf8, s16, "utf-8"));                TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16be, s16, "utf-16be"));          TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16le, s16, "utf-16le"));          TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32be, s16, "utf-32be"));          TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32le, s16, "utf-32le"));          TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_windows1252, s16, "windows-1252"));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_iso8859_15, s16, "iso-8859-15"));    TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_gb18030, s16, "gb18030"));           TEST_EQUAL(s16, euro_utf16);

        TRY(import_string(euro_utf8, s32, "utf-8"));                TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16be, s32, "utf-16be"));          TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16le, s32, "utf-16le"));          TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32be, s32, "utf-32be"));          TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32le, s32, "utf-32le"));          TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_windows1252, s32, "windows-1252"));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_iso8859_15, s32, "iso-8859-15"));    TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_gb18030, s32, "gb18030"));           TEST_EQUAL(s32, euro_utf32);

        TRY(import_string(euro_utf8, s8, 65001));        TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16be, s8, 1201));      TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16le, s8, 1200));      TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32be, s8, 12001));     TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32le, s8, 12000));     TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_windows1252, s8, 1252));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_iso8859_15, s8, 28605));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_gb18030, s8, 54936));     TEST_EQUAL(s8, euro_utf8);

        TRY(import_string(euro_utf8, s16, 65001));        TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16be, s16, 1201));      TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16le, s16, 1200));      TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32be, s16, 12001));     TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32le, s16, 12000));     TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_windows1252, s16, 1252));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_iso8859_15, s16, 28605));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_gb18030, s16, 54936));     TEST_EQUAL(s16, euro_utf16);

        TRY(import_string(euro_utf8, s32, 65001));        TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16be, s32, 1201));      TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16le, s32, 1200));      TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32be, s32, 12001));     TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32le, s32, 12000));     TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_windows1252, s32, 1252));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_iso8859_15, s32, 28605));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_gb18030, s32, 54936));     TEST_EQUAL(s32, euro_utf32);

        TRY(import_string(euro_utf8, s8, "utf-8", err_throw));       TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf8, s8, 65001, err_throw));         TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_gb18030, s8, "gb18030", err_throw));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_gb18030, s8, 54936, err_throw));      TEST_EQUAL(s8, euro_utf8);
        TEST_THROW(import_string(euro_windows1252, s8, "utf-8", err_throw), EncodingError);
        TEST_THROW(import_string(euro_windows1252, s8, 65001, err_throw), EncodingError);

        // Error detection is not reliable on Windows
        #if defined(_XOPEN_SOURCE)
            TEST_THROW(import_string(euro_utf8, s8, "ascii", err_throw), EncodingError);
            TEST_THROW(import_string(euro_utf8, s8, 20127, err_throw), EncodingError);
        #endif

        TRY(import_string(euro_utf8, s16, "utf-8", err_throw));       TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf8, s16, 65001, err_throw));         TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_gb18030, s16, "gb18030", err_throw));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_gb18030, s16, 54936, err_throw));      TEST_EQUAL(s16, euro_utf16);
        TEST_THROW(import_string(euro_windows1252, s16, "utf-8", err_throw), EncodingError);
        TEST_THROW(import_string(euro_windows1252, s16, 65001, err_throw), EncodingError);

        #if defined(_XOPEN_SOURCE)
            TEST_THROW(import_string(euro_utf8, s16, "ascii", err_throw), EncodingError);
            TEST_THROW(import_string(euro_utf8, s16, 20127, err_throw), EncodingError);
        #endif

        TRY(import_string(euro_utf8, s32, "utf-8", err_throw));       TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf8, s32, 65001, err_throw));         TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_gb18030, s32, "gb18030", err_throw));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_gb18030, s32, 54936, err_throw));      TEST_EQUAL(s32, euro_utf32);
        TEST_THROW(import_string(euro_windows1252, s32, "utf-8", err_throw), EncodingError);
        TEST_THROW(import_string(euro_windows1252, s32, 65001, err_throw), EncodingError);

        #if defined(_XOPEN_SOURCE)
            TEST_THROW(import_string(euro_utf8, s32, "ascii", err_throw), EncodingError);
            TEST_THROW(import_string(euro_utf8, s32, 20127, err_throw), EncodingError);
        #endif

        TRY(import_string(euro_utf8, s8, "utf"));     TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16be, s8, "utf"));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf16le, s8, "utf"));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32be, s8, "utf"));  TEST_EQUAL(s8, euro_utf8);
        TRY(import_string(euro_utf32le, s8, "utf"));  TEST_EQUAL(s8, euro_utf8);

        TRY(import_string(euro_utf8, s16, "utf"));     TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16be, s16, "utf"));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf16le, s16, "utf"));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32be, s16, "utf"));  TEST_EQUAL(s16, euro_utf16);
        TRY(import_string(euro_utf32le, s16, "utf"));  TEST_EQUAL(s16, euro_utf16);

        TRY(import_string(euro_utf8, s32, "utf"));     TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16be, s32, "utf"));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf16le, s32, "utf"));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32be, s32, "utf"));  TEST_EQUAL(s32, euro_utf32);
        TRY(import_string(euro_utf32le, s32, "utf"));  TEST_EQUAL(s32, euro_utf32);

    }

    void check_unicode_to_mbcs() {

        std::string s;

        TRY(export_string(euro_utf8, s, "utf-8"));         TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf8, s, "utf-16be"));      TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf8, s, "utf-16le"));      TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf8, s, "utf-32be"));      TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf8, s, "utf-32le"));      TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf8, s, "windows-1252"));  TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf8, s, "iso-8859-15"));   TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf8, s, "gb18030"));       TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf16, s, "utf-8"));         TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf16, s, "utf-16be"));      TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf16, s, "utf-16le"));      TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf16, s, "utf-32be"));      TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf16, s, "utf-32le"));      TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf16, s, "windows-1252"));  TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf16, s, "iso-8859-15"));   TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf16, s, "gb18030"));       TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf32, s, "utf-8"));         TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf32, s, "utf-16be"));      TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf32, s, "utf-16le"));      TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf32, s, "utf-32be"));      TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf32, s, "utf-32le"));      TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf32, s, "windows-1252"));  TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf32, s, "iso-8859-15"));   TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf32, s, "gb18030"));       TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf8, s, 65001));  TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf8, s, 1201));   TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf8, s, 1200));   TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf8, s, 12001));  TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf8, s, 12000));  TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf8, s, 1252));   TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf8, s, 28605));  TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf8, s, 54936));  TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf16, s, 65001));  TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf16, s, 1201));   TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf16, s, 1200));   TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf16, s, 12001));  TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf16, s, 12000));  TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf16, s, 1252));   TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf16, s, 28605));  TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf16, s, 54936));  TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf32, s, 65001));  TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf32, s, 1201));   TEST_EQUAL(s, euro_utf16be);
        TRY(export_string(euro_utf32, s, 1200));   TEST_EQUAL(s, euro_utf16le);
        TRY(export_string(euro_utf32, s, 12001));  TEST_EQUAL(s, euro_utf32be);
        TRY(export_string(euro_utf32, s, 12000));  TEST_EQUAL(s, euro_utf32le);
        TRY(export_string(euro_utf32, s, 1252));   TEST_EQUAL(s, euro_windows1252);
        TRY(export_string(euro_utf32, s, 28605));  TEST_EQUAL(s, euro_iso8859_15);
        TRY(export_string(euro_utf32, s, 54936));  TEST_EQUAL(s, euro_gb18030);

        TRY(export_string(euro_utf8, s, "utf-8", err_throw));    TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf8, s, 65001, err_throw));      TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf8, s, "gb18030", err_throw));  TEST_EQUAL(s, euro_gb18030);
        TRY(export_string(euro_utf8, s, 54936, err_throw));      TEST_EQUAL(s, euro_gb18030);
        TEST_THROW(export_string(euro_utf8 + '\xff', s, "utf-8", err_throw), EncodingError);
        TEST_THROW(export_string(euro_utf8 + '\xff', s, 65001, err_throw), EncodingError);

        #if defined(_XOPEN_SOURCE)
            TEST_THROW(export_string(euro_utf8, s, "ascii", err_throw), EncodingError);
            TEST_THROW(export_string(euro_utf8, s, 20127, err_throw), EncodingError);
        #endif

        TRY(export_string(euro_utf16, s, "utf-8", err_throw));    TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf16, s, 65001, err_throw));      TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf16, s, "gb18030", err_throw));  TEST_EQUAL(s, euro_gb18030);
        TRY(export_string(euro_utf16, s, 54936, err_throw));      TEST_EQUAL(s, euro_gb18030);
        TEST_THROW(export_string(euro_utf16 + char16_t(0xdfff), s, "utf-8", err_throw), EncodingError);
        TEST_THROW(export_string(euro_utf16 + char16_t(0xdfff), s, 65001, err_throw), EncodingError);

        #if defined(_XOPEN_SOURCE)
            TEST_THROW(export_string(euro_utf16, s, "ascii", err_throw), EncodingError);
            TEST_THROW(export_string(euro_utf16, s, 20127, err_throw), EncodingError);
        #endif

        TRY(export_string(euro_utf32, s, "utf-8", err_throw));    TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf32, s, 65001, err_throw));      TEST_EQUAL(s, euro_utf8);
        TRY(export_string(euro_utf32, s, "gb18030", err_throw));  TEST_EQUAL(s, euro_gb18030);
        TRY(export_string(euro_utf32, s, 54936, err_throw));      TEST_EQUAL(s, euro_gb18030);
        TEST_THROW(export_string(euro_utf32 + char32_t(0xdfff), s, "utf-8", err_throw), EncodingError);
        TEST_THROW(export_string(euro_utf32 + char32_t(0xdfff), s, 65001, err_throw), EncodingError);

        #if defined(_XOPEN_SOURCE)
            TEST_THROW(export_string(euro_utf32, s, "ascii", err_throw), EncodingError);
            TEST_THROW(export_string(euro_utf32, s, 20127, err_throw), EncodingError);
        #endif

    }

    void check_local_encoding_round_trip() {

        std::string s;
        u8string s8;

        TRY(export_string(euro_utf8, s, "char"));
        TRY(import_string(s, s8, "char"));
        TEST_EQUAL(s8, euro_utf8);

        TRY(export_string(euro_utf8, s, "char", err_throw));
        TRY(import_string(s, s8, "char", err_throw));
        TEST_EQUAL(s8, euro_utf8);

        TRY(export_string(euro_utf8, s, "wchar_t"));
        TRY(import_string(s, s8, "wchar_t"));
        TEST_EQUAL(s8, euro_utf8);

        TRY(export_string(euro_utf8, s, "wchar_t", err_throw));
        TRY(import_string(s, s8, "wchar_t", err_throw));
        TEST_EQUAL(s8, euro_utf8);

    }

    void check_error_message_translation() {

        u8string s8;

        TRY(s8 = system_message(EDOM));
        TEST(! s8.empty());

        #if defined(_WIN32)
            TRY(s8 = windows_message(ERROR_INVALID_FUNCTION));
            TEST(! s8.empty());
        #endif

    }

}

TEST_MODULE(unicorn, mbcs) {

    check_locale_detection();
    check_utf_detection();
    check_encoding_queries();
    check_mbcs_to_unicode();
    check_unicode_to_mbcs();
    check_local_encoding_round_trip();
    check_error_message_translation();

}
