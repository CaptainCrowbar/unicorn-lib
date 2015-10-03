#include "unicorn/table.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>

using namespace std::literals;

namespace Unicorn {

    constexpr Kwarg<uint32_t> Table::flags;
    constexpr Kwarg<u8string> Table::ditto, Table::empty;
    constexpr Kwarg<size_t> Table::margin, Table::spacing;
    constexpr Kwarg<bool> Table::unfill;

    void Table::character_code(char32_t c) {
        switch (c) {
            case '\f': // eol + reset formats
                force_break();
                formats.clear();
                break;
            case '\n': // end of line
                cells.push_back({});
                break;
            case '\t': // copy from left
                if (cells.back().empty())
                    cells.back().push_back({});
                else
                    cells.back().push_back(cells.back().back());
                break;
            case '\v': // copy from above
                if (cells.size() <= 1)
                    cells.back().push_back({});
                else
                    cells.back().push_back(cells[cells.size() - 2][cells.back().size()]);
                break;
            default: // insert divider
                if (char_is_unassigned(c) || char_is_control(c) || char_is_white_space(c))
                    throw std::invalid_argument("Invalid table divider: "s + char_as_hex(c));
                force_break();
                cells.back().push_back(u8string(1, 0) + str_chars<char>(c));
                cells.push_back({});
                break;
        }
    }

    void Table::force_break() {
        if (! cells.back().empty())
            cells.push_back({});
    }

    void Table::write_table(const layout_spec& spec, vector<u8string>& lines) const {
        const size_t ditto_size = str_length(spec.ditto, spec.flags),
            empty_size = str_length(spec.empty, spec.flags);
        lines.resize(cells.size(), u8string(spec.margin, ' '));
        if (cells.back().empty())
            lines.pop_back();
        size_t columns = 0, rows = lines.size(), width = spec.margin;
        for (auto& row: cells)
            columns = std::max(columns, row.size());
        for (size_t c = 0; c < columns; ++c) {
            size_t cell_size = 1;
            for (size_t r = 0; r < rows; ++r) {
                u8string text;
                size_t text_size = 0;
                if (spec.unfill && r > 0 && c < cells[r].size() && c < cells[r - 1].size() && cells[r][c] == cells[r - 1][c]) {
                    text = spec.ditto;
                    text_size = ditto_size;
                } else if (cells[r].size() == 1 && cells[r][0][0] == 0) {
                    // skip
                } else if (c < cells[r].size() && ! cells[r][c].empty()) {
                    text = cells[r][c];
                    text_size = str_length(text, spec.flags);
                } else if (cells[r].size() != 1 || cells[r][0][0] != 0) {
                    text = spec.empty;
                    text_size = empty_size;
                }
                lines[r] += text;
                cell_size = std::max(cell_size, text_size);
            }
            width += cell_size + spec.spacing;
            for (size_t r = 0; r < rows; ++r) {
                if (cells[r].size() == 1 && cells[r][0][0] == 0)
                    str_append_chars(lines[r], *std::next(utf_begin(cells[r][0])), cell_size);
                str_fix_left_in(lines[r], width, ' ', spec.flags);
            }
        }
        for (auto& line: lines)
            str_trim_right_in(line);
    }

}
