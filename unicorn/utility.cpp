#include "unicorn/utility.hpp"
#include <ctime>
#include <limits>

using namespace std::chrono;
using namespace std::literals;

namespace RS {

    // Date and time functions

    Ustring format_date(std::chrono::system_clock::time_point tp, Uview format, uint32_t flags) {
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        if (format.empty())
            return {};
        auto t = system_clock::to_time_t(tp);
        tm stm = zone == local_zone ? *localtime(&t) : *gmtime(&t);
        Ustring fs(format);
        Ustring result(std::max(2 * format.size(), size_t(100)), '\0');
        auto rc = strftime(&result[0], result.size(), fs.data(), &stm);
        if (rc == 0) {
            result.resize(10 * result.size(), '\0');
            rc = strftime(&result[0], result.size(), fs.data(), &stm);
        }
        result.resize(rc);
        result.shrink_to_fit();
        return result;
    }

    Ustring format_date(std::chrono::system_clock::time_point tp, int prec, uint32_t flags) {
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        Ustring result = format_date(tp, "%Y-%m-%d %H:%M:%S"s, zone);
        if (prec > 0) {
            double sec = to_seconds(tp.time_since_epoch());
            double isec;
            double fsec = modf(sec, &isec);
            Ustring buf(prec + 3, '\0');
            snprintf(&buf[0], buf.size(), "%.*f", prec, fsec);
            result += buf.data() + 1;
        }
        return result;
    }

    std::chrono::system_clock::time_point make_date(int year, int month, int day, int hour, int min, double sec, uint32_t flags) {
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        double isec = 0, fsec = modf(sec, &isec);
        if (fsec < 0) {
            isec -= 1;
            fsec += 1;
        }
        tm stm;
        std::memset(&stm, 0, sizeof(stm));
        stm.tm_sec = int(isec);
        stm.tm_min = min;
        stm.tm_hour = hour;
        stm.tm_mday = day;
        stm.tm_mon = month - 1;
        stm.tm_year = year - 1900;
        stm.tm_isdst = -1;
        time_t t;
        if (zone == local_zone)
            t = std::mktime(&stm);
        else
            #ifdef _XOPEN_SOURCE
                t = timegm(&stm);
            #else
                t = _mkgmtime(&stm);
            #endif
        system_clock::time_point::rep extra(int64_t(fsec * system_clock::time_point::duration(seconds(1)).count()));
        return system_clock::from_time_t(t) + system_clock::time_point::duration(extra);
    }

    // String functions

    std::string ascii_lowercase(std::string_view s) {
        Ustring r(s);
        std::transform(r.begin(), r.end(), r.begin(), ascii_tolower);
        return r;
    }

    std::string ascii_uppercase(std::string_view s) {
        Ustring r(s);
        std::transform(r.begin(), r.end(), r.begin(), ascii_toupper);
        return r;
    }

    std::string ascii_titlecase(std::string_view s) {
        Ustring r(s);
        bool was_alpha = false;
        for (char& c: r) {
            if (was_alpha)
                c = ascii_tolower(c);
            else
                c = ascii_toupper(c);
            was_alpha = ascii_isalpha(c);
        }
        return r;
    }

    std::string ascii_sentencecase(std::string_view s) {
        Ustring r(s);
        bool new_sentence = true, was_break = false;
        for (char& c: r) {
            if (c == '\n' || c == '\f' || c == '\r') {
                if (was_break)
                    new_sentence = true;
                was_break = true;
            } else {
                was_break = false;
                if (c == '.') {
                    new_sentence = true;
                } else if (new_sentence && ascii_isalpha(c)) {
                    c = ascii_toupper(c);
                    new_sentence = false;
                }
            }
        }
        return r;
    }

    unsigned long long binnum(std::string_view str) noexcept {
        std::string s(str);
        return std::strtoull(s.data(), nullptr, 2);
    }

    long long decnum(std::string_view str) noexcept {
        std::string s(str);
        return std::strtoll(s.data(), nullptr, 10);
    }

    unsigned long long hexnum(std::string_view str) noexcept {
        std::string s(str);
        return std::strtoull(s.data(), nullptr, 16);
    }

    double fpnum(std::string_view str) noexcept {
        std::string s(str);
        return std::strtod(s.data(), nullptr);
    }

    namespace {

        std::string quote_string(std::string_view str, bool escape_8bit) {
            std::string result = "\"";
            for (auto c: str) {
                auto b = uint8_t(c);
                switch (c) {
                    case 0:     result += "\\0"; break;
                    case '\t':  result += "\\t"; break;
                    case '\n':  result += "\\n"; break;
                    case '\f':  result += "\\f"; break;
                    case '\r':  result += "\\r"; break;
                    case '\"':  result += "\\\""; break;
                    case '\\':  result += "\\\\"; break;
                    default:
                        if (b <= 0x1f || b == 0x7f || (escape_8bit && b >= 0x80))
                            result += "\\x" + hex(b);
                        else
                            result += c;
                        break;
                }
            }
            result += '\"';
            return result;
        }

    }

    std::string quote(std::string_view str) {
        return quote_string(str, false);
    }

    Ustring bquote(std::string_view str) {
        return quote_string(str, true);
    }

    Ustring roman(int n) {
        static constexpr std::pair<int, const char*> table[] = {
            { 900, "CM" }, { 500, "D" }, { 400, "CD" }, { 100, "C" },
            { 90, "XC" }, { 50, "L" }, { 40, "XL" }, { 10, "X" },
            { 9, "IX" }, { 5, "V" }, { 4, "IV" }, { 1, "I" },
        };
        if (n < 1)
            return {};
        Ustring s(size_t(n / 1000), 'M');
        n %= 1000;
        for (auto& t: table) {
            for (int q = n / t.first; q > 0; --q)
                s += t.second;
            n %= t.first;
        }
        return s;
    }

    int64_t si_to_int(Uview str) {
        using limits = std::numeric_limits<int64_t>;
        static constexpr const char* prefixes = "KMGTPEZY";
        Ustring s(str);
        char* endp = nullptr;
        errno = 0;
        int64_t n = std::strtoll(s.data(), &endp, 10);
        if (errno == ERANGE)
            throw std::range_error("Out of range: " + quote(s));
        if (errno || endp == s.data())
            throw std::invalid_argument("Invalid number: " + quote(s));
        if (ascii_isspace(*endp))
            ++endp;
        if (n && ascii_isalpha(*endp)) {
            auto pp = std::strchr(prefixes, ascii_toupper(*endp));
            if (pp) {
                int64_t steps = pp - prefixes + 1;
                double limit = std::log10(double(limits::max()) / double(std::abs(n))) / 3;
                if (double(steps) > limit)
                    throw std::range_error("Out of range: " + quote(s));
                for (int64_t i = 0; i < steps; ++i)
                    n *= 1000;
            }
        }
        return n;
    }

    double si_to_float(Uview str) {
        using limits = std::numeric_limits<double>;
        static constexpr const char* prefixes = "yzafpnum kMGTPEZY";
        Ustring s(str);
        char* endp = nullptr;
        errno = 0;
        double x = std::strtod(s.data(), &endp);
        if (errno == ERANGE)
            throw std::range_error("Out of range: " + quote(s));
        if (errno || endp == s.data())
            throw std::invalid_argument("Invalid number: " + quote(s));
        if (ascii_isspace(*endp))
            ++endp;
        char c = *endp;
        if (x != 0 && ascii_isalpha(c)) {
            if (c == 'K')
                c = 'k';
            auto pp = std::strchr(prefixes, c);
            if (pp) {
                auto steps = pp - prefixes - 8;
                double limit = std::log10(limits::max() / fabs(x)) / 3;
                if (double(steps) > limit)
                    throw std::range_error("Out of range: " + quote(s));
                x *= std::pow(1000.0, double(steps));
            }
        }
        return x;
    }

    // Version numbers

    Version::Version(const Ustring& s) {
        auto i = s.begin(), j = i, end = s.end();
        while (i != end) {
            j = std::find_if_not(i, end, is_digit);
            if (i == j)
                break;
            Ustring part(i, j);
            ver.push_back(unsigned(std::strtoul(part.data(), nullptr, 10)));
            i = j;
            if (i == end || *i != '.')
                break;
            ++i;
            if (i == end || ! is_digit(*i))
                break;
        }
        j = std::find_if(i, end, is_space);
        suf.assign(i, j);
        trim();
    }

    Ustring Version::str(size_t min_elements, char delimiter) const {
        Ustring s;
        for (auto& v: ver) {
            s += std::to_string(v);
            s += delimiter;
        }
        for (size_t i = ver.size(); i < min_elements; ++i) {
            s += '0';
            s += delimiter;
        }
        if (! s.empty())
            s.pop_back();
        s += suf;
        return s;
    }

    uint32_t Version::to32() const noexcept {
        uint32_t v = 0;
        for (size_t i = 0; i < 4; ++i)
            v = (v << 8) | ((*this)[i] & 0xff);
        return v;
    }

    Version Version::from32(uint32_t n) noexcept {
        Version v;
        for (int i = 24; i >= 0 && n != 0; i -= 8)
            v.ver.push_back((n >> i) & 0xff);
        v.trim();
        return v;
    }

    int Version::compare(const Version& v) const noexcept {
        int c = compare_3way(ver, v.ver);
        if (c)
            return c;
        if (suf.empty() != v.suf.empty())
            return int(suf.empty()) - int(v.suf.empty());
        else
            return compare_3way(suf, v.suf);
    }

}
