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
#include <type_traits>
#include <vector>

namespace Unicorn {

    // Forward declarations

    template <typename C> class BasicLexer;
    template <typename C> struct BasicToken;
    template <typename C> class BasicTokenIterator;
    class SyntaxError;

    using Lexer = BasicLexer<char>;
    using Token = BasicToken<char>;
    using TokenIterator = BasicTokenIterator<char>;
    using TokenList = vector<Token>;

    #if defined(UNICORN_PCRE16)
        using Lexer16 = BasicLexer<char16_t>;
        using Token16 = BasicToken<char16_t>;
        using TokenIterator16 = BasicTokenIterator<char16_t>;
        using TokenList16 = vector<Token16>;
    #endif

    #if defined(UNICORN_PCRE32)
        using Lexer32 = BasicLexer<char32_t>;
        using Token32 = BasicToken<char32_t>;
        using TokenIterator32 = BasicTokenIterator<char32_t>;
        using TokenList32 = vector<Token32>;
    #endif

    #if defined(UNICORN_PCRE_WCHAR)
        using WideLexer = BasicLexer<wchar_t>;
        using WideToken = BasicToken<wchar_t>;
        using WideTokenIterator = BasicTokenIterator<wchar_t>;
        using WideTokenList = vector<WideToken>;
    #endif

    // Exceptions

    class SyntaxError:
    public std::runtime_error {
    public:
        SyntaxError(const u8string& text, size_t offset, const u8string& message = "Syntax error"):
            std::runtime_error(assemble(text, offset, message)),
            bug(make_shared<u8string>(text)), ofs(offset) {}
        const u8string& text() const noexcept { return *bug; }
        size_t offset() const noexcept { return ofs; }
    private:
        shared_ptr<u8string> bug;
        size_t ofs;
        static u8string assemble(const u8string& text, size_t offset, const u8string& message);
    };

    inline u8string SyntaxError::assemble(const u8string& text, size_t offset, const u8string& message) {
        u8string s = message;
        s += " at offset ";
        s += dec(offset);
        s += ": Unexpected ";
        if (text.empty())
            s += "EOF";
        else
            s += quote(text, true);
        return s;
    }

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

    template <typename C>
    class BasicTokenIterator:
    public ForwardIterator<BasicTokenIterator<C>, const BasicToken<char>> {
    public:
        using char_type = C;
        using string_type = basic_string<C>;
        const BasicToken<C>& operator*() const noexcept { return token; }
        BasicTokenIterator& operator++();
        bool operator==(const BasicTokenIterator& rhs) const noexcept
            { return token.offset == rhs.token.offset; }
    private:
        friend class BasicLexer<C>;
        const BasicLexer<C>* lex = nullptr;
        BasicToken<C> token {nullptr, 0, 0, 0};
    };

    template <typename C>
    BasicTokenIterator<C>& BasicTokenIterator<C>::operator++() {
        if (! lex || ! token.text)
            return *this;
        while (token.offset < token.text->size()) {
            token.offset += token.count;
            token.count = 0;
            token.tag = 0;
            if (token.offset >= token.text->size())
                break;
            char32_t u;
            if (lex->mask & rx_byte)
                u = char_to_uint((*token.text)[token.offset]);
            else
                u = *UtfIterator<C>(*token.text, token.offset);
            auto elements = &lex->lexemes;
            if (u < lex->prefix_table.size())
                elements = &lex->prefix_table[u];
            for (auto& elem: *elements) {
                auto count = elem.call(*token.text, token.offset);
                if (count > token.count) {
                    token.count = count;
                    token.tag = elem.tag;
                }
            }
            if (token.count == 0)
                throw SyntaxError(str_char<char>(u), token.offset);
            if (token.tag)
                break;
        }
        return *this;
    }

    // Lexer class

    template <typename C>
    class BasicLexer {
    public:
        using char_type = C;
        using string_type = basic_string<C>;
        using regex_type = BasicRegex<C>;
        using token_type = BasicToken<C>;
        using token_list = vector<token_type>;
        using callback_type = function<size_t(const string_type&, size_t)>;
        using token_iterator = BasicTokenIterator<C>;
        using token_range = Irange<token_iterator>;
        BasicLexer(): BasicLexer(0) {}
        explicit BasicLexer(uint32_t flags):
            lexemes(), prefix_table(flags & rx_byte ? 256 : 128), mask((flags | rx_notempty | rx_partialsoft) & ~ rx_partialhard) {}
        void exact(int tag, const string_type& pattern);
        void exact(int tag, const C* pattern) { exact(tag, cstr(pattern)); }
        void match(int tag, const regex_type& pattern) { add_match(tag, pattern); }
        void match(int tag, const string_type& pattern, uint32_t flags = 0) { add_match(tag, regex_type(pattern, fix_flags(flags))); }
        void match(int tag, const C* pattern, uint32_t flags = 0) { add_match(tag, regex_type(pattern, fix_flags(flags))); }
        void custom(int tag, const callback_type& call);
        token_range operator()(const string_type& text) const;
        void operator()(const string_type& text, token_list& tokens) const;
    private:
        friend class BasicTokenIterator<C>;
        struct element {
            int tag;
            callback_type call;
        };
        using element_list = vector<element>;
        using element_table = vector<element_list>;
        element_list lexemes;
        element_table prefix_table;
        uint32_t mask;
        void add_exact(int tag, const string_type& pattern);
        void add_match(int tag, regex_type pattern);
        uint32_t fix_flags(uint32_t flags) const noexcept { return (flags | mask) & ~ rx_partialhard; }
    };

    template <typename C>
    void BasicLexer<C>::exact(int tag, const string_type& pattern) {
        if (pattern.empty())
            return;
        auto call = [pattern] (const string_type& text, size_t offset) -> size_t {
            if (offset < text.size() && text.size() - offset >= pattern.size()
                    && memcmp(text.data() + offset, pattern.data(), pattern.size()) == 0)
                return pattern.size();
            else
                return 0;
        };
        element e = {tag, call};
        lexemes.push_back(e);
        auto u = char_to_uint(pattern[0]);
        if (u < prefix_table.size())
            prefix_table[u].push_back(e);
    }

    template <typename C>
    void BasicLexer<C>::add_match(int tag, regex_type pattern) {
        if (pattern.empty())
            return;
        uint32_t new_flags = fix_flags(pattern.flags());
        if (new_flags != pattern.flags())
            pattern = regex_type(pattern.pattern(), new_flags);
        auto call = [pattern] (const string_type& text, size_t offset) { return pattern.anchor(text, offset).count(); };
        element e = {tag, call};
        lexemes.push_back(e);
        string_type s;
        for (size_t i = 0; i < prefix_table.size(); ++i) {
            s = str_char<C>(i);
            if (pattern.anchor(s).full_or_partial())
                prefix_table[i].push_back(e);
        }
    }

    template <typename C>
    void BasicLexer<C>::custom(int tag, const callback_type& call) {
        if (! call)
            return;
        element e = {tag, call};
        lexemes.push_back(e);
        for (auto& p: prefix_table)
            p.push_back(e);
    }

    template <typename C>
    typename BasicLexer<C>::token_range BasicLexer<C>::operator()(const string_type& text) const {
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

    template <typename C>
    void BasicLexer<C>::operator()(const string_type& text, token_list& tokens) const {
        auto range = (*this)(text);
        token_list list(range.begin(), range.end());
        tokens = move(list);
    }

}
