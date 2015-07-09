#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Unicorn {

    // Forward declarations

    template <typename CX> class BasicLexer;
    template <typename CX> struct BasicToken;
    template <typename CX> class BasicTokenIterator;
    class SyntaxError;

    using Lexer = BasicLexer<char>;
    using Token = BasicToken<char>;
    using TokenIterator = BasicTokenIterator<char>;
    using ByteLexer = BasicLexer<ByteMode>;
    using ByteTokenIterator = BasicTokenIterator<ByteMode>;

    #if defined(UNICORN_PCRE16)
        using Lexer16 = BasicLexer<char16_t>;
        using Token16 = BasicToken<char16_t>;
        using TokenIterator16 = BasicTokenIterator<char16_t>;
    #endif

    #if defined(UNICORN_PCRE32)
        using Lexer32 = BasicLexer<char32_t>;
        using Token32 = BasicToken<char32_t>;
        using TokenIterator32 = BasicTokenIterator<char32_t>;
    #endif

    #if defined(UNICORN_PCRE_WCHAR)
        using WideLexer = BasicLexer<wchar_t>;
        using WideToken = BasicToken<wchar_t>;
        using WideTokenIterator = BasicTokenIterator<wchar_t>;
    #endif

    // Exceptions

    class SyntaxError:
    public std::runtime_error {
    public:
        SyntaxError(const u8string& text, size_t offset, const u8string& message = "Syntax error");
        u8string text() const { return bug; }
        size_t offset() const { return ofs; }
    private:
        u8string bug;
        size_t ofs;
    };

    // Token structure

    template <typename C>
    struct BasicToken {
        using char_type = C;
        using string_type = basic_string<C>;
        const string_type* text;
        size_t offset;
        size_t count;
        int tag;
        operator string_type() const { return text ? text->substr(offset, count) : string_type(); }
    };

    // Token iterator

    template <typename CX>
    class BasicTokenIterator {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using string_type = basic_string<char_type>;
        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const BasicToken<char_type>*;
        using reference = const BasicToken<char_type>&;
        using value_type = BasicToken<char_type>;
        const value_type& operator*() noexcept { return token; }
        const value_type* operator->() noexcept { return &**this; }
        BasicTokenIterator& operator++();
        BasicTokenIterator operator++(int) { auto i = *this; ++*this; return i; }
        bool operator==(const BasicTokenIterator& rhs) const noexcept
            { return token.offset == rhs.token.offset; }
        bool operator!=(const BasicTokenIterator& rhs) const noexcept { return ! (*this == rhs); }
    private:
        friend class BasicLexer<CX>;
        const BasicLexer<CX>* lex = nullptr;
        value_type token {nullptr, 0, 0, 0};
    };

    template <typename CX>
    BasicTokenIterator<CX>& BasicTokenIterator<CX>::operator++() {
        if (! lex || ! token.text)
            return *this;
        while (token.offset < token.text->size()) {
            token.offset += token.count;
            token.count = 0;
            token.tag = 0;
            if (token.offset >= token.text->size())
                break;
            auto it = UnicornDetail::SubjectIterator<CX>::make_iterator(*token.text, token.offset);
            auto elements = &lex->lexemes;
            if (as_uchar(*it) < lex->prefix_table.size())
                elements = &lex->prefix_table[as_uchar(*it)];
            for (auto& elem: *elements) {
                auto count = elem.call(*token.text, token.offset);
                if (count > token.count) {
                    token.count = count;
                    token.tag = elem.tag;
                }
            }
            if (token.count == 0)
                throw SyntaxError(str_chars<char>(as_uchar(*it)), token.offset);
            if (token.tag)
                break;
        }
        return *this;
    }

    // Lexer class

    template <typename CX>
    class BasicLexer {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using string_type = basic_string<char_type>;
        using regex_type = BasicRegex<CX>;
        using token_type = BasicToken<char_type>;
        using callback_type = std::function<size_t(const string_type&, size_t)>;
        using token_iterator = BasicTokenIterator<CX>;
        using token_range = Crow::Irange<token_iterator>;
        BasicLexer(): lexemes(), prefix_table(prefix_count) {}
        token_range operator()(const string_type& text) const { return lex(text); }
        token_range lex(const string_type& text) const;
        void call(int tag, const callback_type& call);
        void exact(int tag, const string_type& pattern);
        void exact(int tag, const char_type* pattern) { exact(tag, Crow::cstr(pattern)); }
        void match(int tag, const regex_type& pattern) { add_match(tag, pattern); }
        void match(int tag, const string_type& pattern, Crow::Flagset flags = {})
            { add_match(tag, regex_type(pattern, flags)); }
        void match(int tag, const char_type* pattern, Crow::Flagset flags = {})
            { add_match(tag, regex_type(pattern, flags)); }
    private:
        friend class BasicTokenIterator<CX>;
        static constexpr size_t prefix_count = std::is_same<CX, ByteMode>::value ? 256 : 128;
        struct element {
            int tag;
            callback_type call;
        };
        using element_list = std::vector<element>;
        using element_table = std::vector<element_list>;
        element_list lexemes;
        element_table prefix_table;
        void add_exact(int tag, const string_type& pattern);
        void add_match(int tag, regex_type pattern);
    };

    template <typename CX>
    typename BasicLexer<CX>::token_range BasicLexer<CX>::lex(const string_type& text) const {
        token_range result;
        result.first.lex = result.second.lex = this;
        result.first.token.text = result.second.token.text = &text;
        result.first.token.offset = 0;
        result.second.token.offset = text.size();
        result.first.token.count = result.second.token.count = 0;
        result.first.token.tag = result.second.token.tag = 0;
        ++result.first;
        return result;
    }

    template <typename CX>
    void BasicLexer<CX>::call(int tag, const callback_type& call) {
        if (! call)
            return;
        lexemes.push_back(element{tag, call});
        for (auto& p: prefix_table)
            p.push_back(lexemes.back());
    }

    template <typename CX>
    void BasicLexer<CX>::exact(int tag, const string_type& pattern) {
        if (pattern.empty())
            return;
        auto call = [pattern] (const string_type& text, size_t offset) -> size_t {
            if (offset < text.size() && text.size() - offset >= pattern.size()
                    && memcmp(text.data() + offset, pattern.data(), pattern.size()) == 0)
                return pattern.size();
            else
                return 0;
        };
        lexemes.push_back(element{tag, call});
        auto i = as_uchar(pattern[0]);
        if (i < prefix_count)
            prefix_table[i].push_back(lexemes.back());
    }

    template <typename CX>
    void BasicLexer<CX>::add_match(int tag, regex_type pattern) {
        if (pattern.empty())
            return;
        Crow::Flagset new_flags = (pattern.flags() | rx_notempty | rx_partialsoft) & ~ rx_partialhard;
        if (new_flags != pattern.flags())
            pattern = regex_type(pattern.pattern(), new_flags);
        auto call = [pattern] (const string_type& text, size_t offset)
            { return pattern.anchor(text, offset).count(); };
        lexemes.push_back(element{tag, call});
        string_type s(1, 0);
        for (size_t i = 0; i < prefix_count; ++i) {
            s[0] = char_type(i);
            if (pattern.anchor(s).full_or_partial())
                prefix_table[i].push_back(lexemes.back());
        }
    }

}
