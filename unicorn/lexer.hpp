#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace Unicorn {

    // Forward declarations

    class Lexer;
    struct Token;
    class TokenIterator;
    class SyntaxError;

    // Exceptions

    class SyntaxError:
    public std::runtime_error {
    public:
        SyntaxError(const u8string& text, size_t offset, const u8string& message = "Syntax error");
        const u8string& text() const noexcept { return *bug; }
        size_t offset() const noexcept { return ofs; }
    private:
        shared_ptr<u8string> bug;
        size_t ofs;
        static u8string assemble(const u8string& text, size_t offset, const u8string& message);
    };

    // Token structure

    struct Token {
        const u8string* text;
        size_t offset;
        size_t count;
        int tag;
        operator u8string() const { return text && offset < text->size() ? text->substr(offset, count) : u8string(); }
    };

    // Token iterator

    class TokenIterator:
    public ForwardIterator<TokenIterator, const Token> {
    public:
        const Token& operator*() const noexcept { return token; }
        TokenIterator& operator++();
        bool operator==(const TokenIterator& rhs) const noexcept { return token.offset == rhs.token.offset; }
    private:
        friend class Lexer;
        const Lexer* lex = nullptr;
        Token token = {nullptr, 0, 0, 0};
    };

    // Lexer class

    class Lexer {
    public:
        using callback_type = function<size_t(const u8string&, size_t)>;
        Lexer(): Lexer(0) {}
        explicit Lexer(uint32_t flags);
        void exact(int tag, const u8string& pattern);
        void exact(int tag, const char* pattern) { exact(tag, cstr(pattern)); }
        void match(int tag, const Regex& pattern) { add_match(tag, pattern); }
        void match(int tag, const u8string& pattern, uint32_t flags = 0) { add_match(tag, Regex(pattern, fix_flags(flags))); }
        void match(int tag, const char* pattern, uint32_t flags = 0) { add_match(tag, Regex(pattern, fix_flags(flags))); }
        void custom(int tag, const callback_type& call);
        Irange<TokenIterator> operator()(const u8string& text) const;
        void operator()(const u8string& text, vector<Token>& tokens) const;
    private:
        friend class TokenIterator;
        struct element {
            int tag;
            callback_type call;
        };
        using element_list = vector<element>;
        using element_table = vector<element_list>;
        element_list lexemes;
        element_table prefix_table;
        uint32_t mask;
        void add_exact(int tag, const u8string& pattern);
        void add_match(int tag, Regex pattern);
        uint32_t fix_flags(uint32_t flags) const noexcept { return (flags | mask) & ~ rx_partialhard; }
    };

}
