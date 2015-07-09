#include "unicorn/lexer.hpp"
#include "unicorn/format.hpp"

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    // Exceptions

    namespace {

        u8string syntax_error_message(const u8string& text, size_t offset, const u8string& message) {
            static const auto format = "$1 at offset $2: Unexpected "_fmt;
            auto s = format(message, offset);
            if (text.empty())
                s += "EOF";
            else
                format_type(text, s, fx_ascquote);
            return s;
        }

    }

    SyntaxError::SyntaxError(const u8string& text, size_t offset, const u8string& message):
    std::runtime_error(syntax_error_message(text, offset, message)), bug(text), ofs(offset) {}

}
