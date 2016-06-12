#include "unicorn/lexer.hpp"

namespace Unicorn {

    // Exceptions

    SyntaxError::SyntaxError(const u8string& text, size_t offset, const u8string& message):
    std::runtime_error(assemble(text, offset, message)),
    bug(make_shared<u8string>(text)),
    ofs(offset) {}

    u8string SyntaxError::assemble(const u8string& text, size_t offset, const u8string& message) {
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

    // Token iterator

    TokenIterator& TokenIterator::operator++() {
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
                u = *utf_iterator(*token.text, token.offset);
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
                throw SyntaxError(str_char(u), token.offset);
            if (token.tag)
                break;
        }
        return *this;
    }

    // Lexer class

    Lexer::Lexer(uint32_t flags):
    lexemes(),
    prefix_table(flags & rx_byte ? 256 : 128),
    mask((flags | rx_notempty | rx_partialsoft) & ~ rx_partialhard) {}

    void Lexer::exact(int tag, const u8string& pattern) {
        if (pattern.empty())
            return;
        auto call = [pattern] (const u8string& text, size_t offset) -> size_t {
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

    void Lexer::add_match(int tag, Regex pattern) {
        if (pattern.empty())
            return;
        uint32_t new_flags = fix_flags(pattern.flags());
        if (new_flags != pattern.flags())
            pattern = Regex(pattern.pattern(), new_flags);
        auto call = [pattern] (const u8string& text, size_t offset) { return pattern.anchor(text, offset).count(); };
        element e = {tag, call};
        lexemes.push_back(e);
        u8string s;
        for (size_t i = 0; i < prefix_table.size(); ++i) {
            s = str_char(i);
            if (pattern.anchor(s).full_or_partial())
                prefix_table[i].push_back(e);
        }
    }

    void Lexer::custom(int tag, const callback_type& call) {
        if (! call)
            return;
        element e = {tag, call};
        lexemes.push_back(e);
        for (auto& p: prefix_table)
            p.push_back(e);
    }

    Irange<TokenIterator> Lexer::operator()(const u8string& text) const {
        Irange<TokenIterator> result;
        result.first.lex = result.second.lex = this;
        result.first.token.text = result.second.token.text = &text;
        result.first.token.offset = 0;
        result.second.token.offset = text.size();
        result.first.token.count = result.second.token.count = 0;
        result.first.token.tag = result.second.token.tag = 0;
        ++result.first;
        return result;
    }

    void Lexer::operator()(const u8string& text, vector<Token>& tokens) const {
        auto range = (*this)(text);
        vector<Token> list(range.begin(), range.end());
        tokens = move(list);
    }

}
