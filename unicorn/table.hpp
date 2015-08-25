#pragma once

#include "unicorn/core.hpp"
#include "unicorn/format.hpp"
#include "unicorn/string.hpp"
#include <ostream>
#include <string>
#include <vector>

namespace Unicorn {

    class Table {
    public:
        static constexpr Kwarg<Flagset> flags = {};
        static constexpr Kwarg<u8string> ditto = {}, empty = {};
        static constexpr Kwarg<size_t> margin = {}, spacing = {};
        static constexpr Kwarg<bool> unfill = {};
        Table(): formats(), cells(1) {}
        template <typename T> Table& operator<<(const T& t);
        Table& operator<<(char c) { return *this << as_uchar(c); }
        Table& operator<<(char16_t c) { return *this << as_uchar(c); }
        Table& operator<<(char32_t c);
        Table& operator<<(wchar_t c) { return *this << as_uchar(c); }
        void clear() noexcept { cells.clear(); cells.resize(1); formats.clear(); }
        template <typename... FS> void format(const u8string& f, const FS&... fs)
            { format(f); format(fs...); }
        void format(const u8string& f) { formats.push_back(Unicorn::format(f)); }
        template <typename C, typename... Args> std::basic_string<C> as_string(const Args&... args) const;
        template <typename... Args> u8string str(const Args&... args) const { return as_string<char>(args...); }
        template <typename C, typename... Args> void write(std::basic_ostream<C>& out, const Args&... args) const;
    private:
        struct layout_spec {
            Flagset flags = grapheme_units;
            u8string ditto = "''", empty = "--";
            size_t margin = 0, spacing = 2;
            bool unfill = false;
        };
        std::vector<Format> formats;
        std::vector<std::vector<u8string>> cells;
        void force_break();
        template <typename... Args> static layout_spec parse_args(const Args&... args);
        void write_table(const layout_spec& spec, std::vector<u8string>& lines) const;
    };

    template <typename T>
    Table& Table::operator<<(const T& t) {
        size_t index = cells.back().size();
        u8string cell;
        if (index < formats.size() && ! formats[index].format().empty())
            cell = formats[index](t);
        else
            cell = to_str(t);
        cells.back().push_back(str_trim(cell));
        return *this;
    }

    template <typename C, typename... Args>
    std::basic_string<C> Table::as_string(const Args&... args) const {
        auto spec = parse_args(args...);
        std::vector<u8string> lines;
        write_table(spec, lines);
        std::basic_string<C> result;
        for (auto& line: lines) {
            result += recode<C>(line);
            result += C('\n');
        }
        return result;
    }

    template <typename C, typename... Args>
    void Table::write(std::basic_ostream<C>& out, const Args&... args) const {
        auto spec = parse_args(args...);
        std::vector<u8string> lines;
        write_table(spec, lines);
        for (auto& line: lines)
            out << recode<C>(line) << C('\n');
    }

    template <typename... Args>
    Table::layout_spec Table::parse_args(const Args&... args) {
        layout_spec spec;
        kwget(flags, spec.flags, args...);
        kwget(ditto, spec.ditto, args...);
        kwget(empty, spec.empty, args...);
        kwget(margin, spec.margin, args...);
        kwget(spacing, spec.spacing, args...);
        kwget(unfill, spec.unfill, args...);
        return spec;
    }

    template <typename C>
    std::basic_ostream<C>& operator<<(std::basic_ostream<C>& out, const Table& tab) {
        tab.write(out);
        return out;
    }

}
