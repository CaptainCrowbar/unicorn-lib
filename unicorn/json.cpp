#include "unicorn/json.hpp"
#include "unicorn/format.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace Json {

        namespace {

            bool expect_str(const u8string& expected, const u8string& src, size_t& pos, bool check) {
                size_t esize = expected.size(), ssize = src.size();
                if (pos <= ssize && ssize - pos >= esize && memcmp(src.data() + pos, expected.data(), esize) == 0) {
                    pos += esize;
                    return true;
                } else if (check) {
                    throw BadJson(pos);
                } else {
                    return false;
                }
            }

            void expect_ws(const u8string& src, size_t& pos, bool check) {
                pos = src.find_first_not_of("\t\n\r "s, pos);
                if (pos == npos && check)
                    throw EndOfFile(src.size());
            }

            char16_t read_utf16_unit(const u8string& src, size_t pos) {
                if (src.size() - pos < 6 || src[pos] != '\\' || src[pos + 1] != 'u')
                    throw BadJson(pos);
                for (size_t i = 2; i < 6; ++i)
                    if (! ascii_isxdigit(src[pos + i]))
                        throw BadJson(pos);
                return char16_t(hexnum(src.substr(pos + 2, 4)));
            }

            void read_utf16_char(const u8string& src, size_t& pos, u8string& dst) {
                // UTF-16 surrogates are d800-dbff and dc00-dfff
                u16string u(2, 0);
                u[0] = read_utf16_unit(src, pos);
                if (u[0] <= 0xd7ff || u[0] >= 0xe000) {
                    str_append_char(dst, u[0]);
                    pos += 6;
                    return;
                }
                if (u[0] <= 0xdbff) {
                    u[1] = read_utf16_unit(src, pos + 6);
                    if (u[1] >= 0xdc00 && u[0] <= 0xdfff) {
                        str_append(dst, u);
                        pos += 12;
                        return;
                    }
                }
                throw BadJson(pos);
            }

            void read_utf8_string(const u8string& src, size_t& pos, u8string& dst) {
                size_t size = src.size();
                ++pos;
                dst.clear();
                for (;;) {
                    size_t next = src.find_first_of("\"\\", pos);
                    if (next == npos)
                        throw EndOfFile(size);
                    dst.append(src, pos, next - pos);
                    if (src[next] == '\"') {
                        pos = next + 1;
                        break;
                    } else if (src[next + 1] == 'u') {
                        read_utf16_char(src, pos, dst);
                    } else {
                        if (size - next < 3)
                            throw EndOfFile(size);
                        switch (src[next + 1]) {
                            case 'b':  dst += '\b'; break;
                            case 't':  dst += '\t'; break;
                            case 'n':  dst += '\n'; break;
                            case 'f':  dst += '\f'; break;
                            case 'r':  dst += '\r'; break;
                            default:   dst += src[next + 1]; break;
                        }
                        pos = next + 2;
                    }
                }
            }

            void write_utf8_string(const u8string& src, u8string& dst) {
                size_t dsize = dst.size();
                ScopeFailure rewind([&dst,dsize] { dst.resize(dsize); });
                dst += '\"';
                size_t i = 0, size = src.size();
                while (i < size) {
                    size_t j = i;
                    while (j < size && uint8_t(src[j]) >= 32 && src[j] != '\"' && src[j] != '\\')
                        ++j;
                    dst.append(src, i, j - i);
                    if (j == size)
                        break;
                    switch (src[j]) {
                        case '\b':  dst += "\\b"; break;
                        case '\t':  dst += "\\t"; break;
                        case '\n':  dst += "\\n"; break;
                        case '\f':  dst += "\\f"; break;
                        case '\r':  dst += "\\r"; break;
                        case '\"':  dst += "\\\""; break;
                        case '\\':  dst += "\\\\"; break;
                        default:
                            dst += "\\u00";
                            dst += hex(uint8_t(src[j]), 2);
                            break;
                    }
                    i = j + 1;
                }
                dst += '\"';
            }

            bool array_is_short(const Array& array, size_t max_array) noexcept {
                return array.size() <= max_array
                    && std::find_if(array.begin(), array.end(),
                        [] (auto& e) { return e.type() >= Json::array; })
                        == array.end();
            }

            void layout_helper(const Element& e, std::ostream& out, size_t max_array,
                    size_t depth, bool indent, const char* suffix) {
                u8string dent(4 * depth, ' ');
                if (indent)
                    out << dent;
                if (e.type() == Json::array) {
                    out << "[";
                    if (! e.array().empty()) {
                        if (array_is_short(e.array(), max_array)) {
                            auto i = e.array().begin(), j = std::prev(e.array().end());
                            for (; i != j; ++i)
                                layout_helper(*i, out, max_array, depth + 1, false, ",");
                            layout_helper(*j, out, max_array, depth + 1, false, "");
                        } else {
                            out << "\n";
                            auto i = e.array().begin(), j = std::prev(e.array().end());
                            for (; i != j; ++i)
                                layout_helper(*i, out, max_array, depth + 1, true, ",\n");
                            layout_helper(*j, out, max_array, depth + 1, true, "\n");
                            out << dent;
                        }
                    }
                    out << "]";
                } else if (e.type() == Json::object) {
                    out << "{";
                    if (! e.object().empty()) {
                        out << "\n";
                        auto i = e.object().begin(), j = std::prev(e.object().end());
                        for (; i != j; ++i) {
                            layout_helper(i->first, out, max_array, depth + 1, true, ": ");
                            layout_helper(i->second, out, max_array, depth + 1, false, ",\n");
                        }
                        layout_helper(j->first, out, max_array, depth + 1, true, ": ");
                        layout_helper(j->second, out, max_array, depth + 1, false, "\n");
                        out << dent;
                    }
                    out << "}";
                } else {
                    out << e;
                }
                out << suffix;
            }

        }

        // Exceptions

        u8string Exception::assemble(const u8string& message, size_t pos) {
            u8string s = message;
            if (pos != npos)
                s += " at offset " + dec(pos);
            return s;
        }

        BadJson::BadJson(size_t pos): Exception("Invalid JSON", pos) {}
        EndOfFile::EndOfFile(size_t pos): Exception("Unexpected end of text", pos) {}
        WrongType::WrongType(): Exception("Wrong JSON type") {}

        // Class Element

        Element::Element(const Element& e):
        etype(e.etype) {
            switch (etype) {
                case Json::string:  rep.string = new u8string(*e.rep.string); break;
                case Json::array:   rep.array = new Array(*e.rep.array); break;
                case Json::object:  rep.object = new Object(*e.rep.object); break;
                default:            memcpy(&rep, &e.rep, rep_size); break;
            }
        }

        Element::Element(Element&& e) noexcept:
        etype(e.etype) {
            memcpy(&rep, &e.rep, rep_size);
            e.etype = Json::null;
        }

        Element& Element::operator=(const Element& e) {
            Element temp(e);
            *this = std::move(temp);
            return *this;
        }

        Element& Element::operator=(Element&& e) noexcept {
            make_null();
            etype = e.etype;
            memcpy(&rep, &e.rep, rep_size);
            e.etype = Json::null;
            return *this;
        }

        Element& Element::operator[](size_t key) {
            check_type(Json::array);
            if (key >= rep.array->size())
                rep.array->resize(key + 1);
            return (*rep.array)[key];
        }

        const Element& Element::operator[](size_t key) const {
            static const Element dummy;
            check_type(Json::array);
            if (key < rep.array->size())
                return (*rep.array)[key];
            else
                return dummy;
        }

        Element& Element::operator[](const u8string& key) {
            check_type(Json::object);
            return (*rep.object)[key];
        }

        const Element& Element::operator[](const u8string& key) const {
            static const Element dummy;
            check_type(Json::object);
            auto it = rep.object->find(key);
            if (it == rep.object->end())
                return dummy;
            else
                return it->second;
        }

        void Element::layout(std::ostream& out, size_t max_array) const {
            layout_helper(*this, out, max_array, 0, true, "\n");
        }

        void Element::write(u8string& dst) const {
            switch (etype) {
                case Json::boolean:  write_boolean(dst); break;
                case Json::number:   write_number(dst); break;
                case Json::string:   write_string(dst); break;
                case Json::array:    write_array(dst); break;
                case Json::object:   write_object(dst); break;
                default:             write_null(dst); break;
            }
        }

        Element Element::read(const u8string& src) {
            size_t pos = 0;
            Element e = read(src, pos);
            expect_ws(src, pos, false);
            if (pos < src.size())
                throw BadJson(pos);
            return e;
        }

        Element Element::read(const u8string& src, size_t& pos) {
            size_t p = pos;
            expect_ws(src, p, true);
            Element e;
            if (src[p] == 'n')
                e.read_null(src, p);
            else if (src[p] == 't' || src[p] == 'f')
                e.read_boolean(src, p);
            else if (src[p] == '-' || (src[p] >= '0' && src[p] <= '9'))
                e.read_number(src, p);
            else if (src[p] == '\"')
                e.read_string(src, p);
            else if (src[p] == '[')
                e.read_array(src, p);
            else if (src[p] == '{')
                e.read_object(src, p);
            else
                throw BadJson(p);
            pos = p;
            return e;
        }

        bool operator==(const Element& lhs, const Element& rhs) noexcept {
            if (lhs.etype != rhs.etype)
                return false;
            switch (lhs.etype) {
                case Json::boolean:  return lhs.rep.boolean == rhs.rep.boolean;
                case Json::number:   return lhs.rep.number == rhs.rep.number;
                case Json::string:   return *lhs.rep.string == *rhs.rep.string;
                case Json::array:    return *lhs.rep.array == *rhs.rep.array;
                case Json::object:   return *lhs.rep.object == *rhs.rep.object;
                default:             return true;
            }
        }

        void Element::make_null() noexcept {
            switch (etype) {
                case Json::string:  delete rep.string; break;
                case Json::array:   delete rep.array; break;
                case Json::object:  delete rep.object; break;
                default:            break;
            }
            etype = Json::null;
        }

        void Element::read_null(const u8string& src, size_t& pos) {
            expect_str("null"s, src, pos, true);
        }

        void Element::read_boolean(const u8string& src, size_t& pos) {
            if (expect_str("true"s, src, pos, false))
                rep.boolean = true;
            else if (expect_str("false"s, src, pos, false))
                rep.boolean = false;
            else
                throw BadJson(pos);
            etype = Json::boolean;
        }

        void Element::read_number(const u8string& src, size_t& pos) {
            static const auto match_number = "-?(0|[1-9]\\d*)(\\.\\d+)?([Ee][+-]?\\d+)?"_re;
            auto match = match_number.anchor(src, pos);
            if (! match)
                throw BadJson(pos);
            etype = Json::number;
            rep.number = fpnum(match.str());
            pos += match.count();
        }

        void Element::read_string(const u8string& src, size_t& pos) {
            auto dst = make_unique<u8string>();
            size_t p = pos;
            read_utf8_string(src, p, *dst);
            etype = Json::string;
            rep.string = dst.get();
            dst.release();
            pos = p;
        }

        void Element::read_array(const u8string& src, size_t& pos) {
            auto dst = make_unique<Array>();
            size_t p = pos + 1;
            expect_ws(src, p, true);
            if (src[p] != ']') {
                for (;;) {
                    dst->push_back(read(src, p));
                    expect_ws(src, p, true);
                    if (src[p] == ']')
                        break;
                    expect_str(","s, src, p, true);
                    expect_ws(src, p, true);
                }
            }
            ++p;
            etype = Json::array;
            rep.array = dst.get();
            dst.release();
            pos = p;
        }

        void Element::read_object(const u8string& src, size_t& pos) {
            auto dst = make_unique<Object>();
            size_t p = pos + 1;
            expect_ws(src, p, true);
            u8string key;
            Element value;
            if (src[p] != '}') {
                for (;;) {
                    read_utf8_string(src, p, key);
                    expect_ws(src, p, true);
                    expect_str(":"s, src, p, true);
                    expect_ws(src, p, true);
                    value = read(src, p);
                    dst->insert({key, value});
                    expect_ws(src, p, true);
                    if (src[p] == '}')
                        break;
                    expect_str(","s, src, p, true);
                    expect_ws(src, p, true);
                }
            }
            ++p;
            etype = Json::object;
            rep.object = dst.get();
            dst.release();
            pos = p;
        }

        void Element::write_null(u8string& dst) const {
            dst += "null";
        }

        void Element::write_boolean(u8string& dst) const {
            dst += rep.boolean ? "true" : "false";
        }

        void Element::write_number(u8string& dst) const {
            static const int precision = std::numeric_limits<double>::max_digits10;
            dst += format_type(rep.number, fx_stripz, precision);
        }

        void Element::write_string(u8string& dst) const {
            write_utf8_string(*rep.string, dst);
        }

        void Element::write_array(u8string& dst) const {
            size_t dsize = dst.size();
            ScopeFailure rewind([&dst,dsize] { dst.resize(dsize); });
            const Array& src(*rep.array);
            if (src.empty()) {
                dst += "[]";
                return;
            }
            dst += '[';
            for (auto& item: src) {
                item.write(dst);
                dst += ',';
            }
            dst.back() = ']';
        }

        void Element::write_object(u8string& dst) const {
            size_t dsize = dst.size();
            ScopeFailure rewind([&dst,dsize] { dst.resize(dsize); });
            const Object& src(*rep.object);
            if (src.empty()) {
                dst += "{}";
                return;
            }
            dst += '{';
            for (auto& pair: src) {
                write_utf8_string(pair.first, dst);
                dst += ':';
                pair.second.write(dst);
                dst += ',';
            }
            dst.back() = '}';
        }

    }

}
