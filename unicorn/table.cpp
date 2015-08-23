#include "unicorn/table.hpp"
#include <algorithm>

using namespace std::literals;

namespace Unicorn {

    constexpr Table::endl_type Table::endl;
    constexpr Kwarg<Flagset> Table::flags;
    constexpr Kwarg<u8string> Table::ditto, Table::empty;
    constexpr Kwarg<size_t> Table::margin, Table::spacing;
    constexpr Kwarg<bool> Table::unfill;

    Table& Table::operator<<(endl_type) {
        cells.push_back({});
        return *this;
    }

    void Table::div(char c) {
        if (! cells.back().empty())
            cells.push_back({});
        cells.back().push_back("\t"s + c);
        cells.push_back({});
    }

    void Table::write_table(const layout_spec& spec, std::vector<u8string>& lines) const {
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
                } else if (cells[r].size() == 1 && cells[r][0][0] == '\t') {
                    // skip
                } else if (c < cells[r].size() && ! cells[r][c].empty()) {
                    text = cells[r][c];
                    text_size = str_length(text, spec.flags);
                } else if (cells[r].size() != 1 || cells[r][0][0] != '\t') {
                    text = spec.empty;
                    text_size = empty_size;
                }
                lines[r] += text;
                cell_size = std::max(cell_size, text_size);
            }
            width += cell_size + spec.spacing;
            for (size_t r = 0; r < rows; ++r) {
                if (cells[r].size() == 1 && cells[r][0][0] == '\t')
                    lines[r].append(cell_size, cells[r][0][1]);
                str_fix_left_in(lines[r], width, ' ', spec.flags);
            }
        }
        for (auto& line: lines)
            str_trim_right_in(line);
    }

}
