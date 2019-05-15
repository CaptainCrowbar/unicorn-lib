#include "unicorn/io.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/path.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <system_error>

using namespace RS::Unicorn::Literals;
using namespace std::literals;

namespace RS::Unicorn {

    namespace {

        using NC = NativeCharacter;
        using SharedFile = std::shared_ptr<FILE>;

        void checked_fclose(FILE* f) { if (f) fclose(f); }
        void null_delete(void*) noexcept {}

        SharedFile shared_fopen(const Path& file, const NativeString& mode, bool check) {
            FILE* f =
                #ifdef _XOPEN_SOURCE
                    fopen
                #else
                    _wfopen
                #endif
                (file.c_name(), mode.data());
            int error = errno;
            if (check && f == nullptr)
                throw std::system_error(error, std::generic_category(), quote(file.name()));
            return {f, checked_fclose};
        }

    }

    // Class FileReader

    struct FileReader::impl_type {
        Ustring line8;
        std::string rdbuf;
        Path file;
        uint32_t flags;
        Ustring enc;
        Ustring eol;
        SharedFile handle;
        size_t lines;
        bool is_ready() noexcept { return handle.get() && ! ferror(handle.get()) && ! feof(handle.get()); }
    };

    const Ustring& FileReader::operator*() const noexcept {
        static const Ustring dummy;
        return impl ? impl->line8 : dummy;
    }

    FileReader& FileReader::operator++() {
        do getline();
            while (impl && (impl->flags & IO::notempty)
                && std::all_of(utf_begin(impl->line8), utf_end(impl->line8), char_is_line_break));
        return *this;
    }

    size_t FileReader::line() const noexcept {
        return impl ? impl->lines : size_t(0);
    }

    void FileReader::init(const Path& file, uint32_t flags, const Ustring& enc, const Ustring& eol) {
        static constexpr NC dash[] = {NC('-'), NC(0)};
        static constexpr NC rb[] = {NC('r'), NC('b'), NC(0)};
        if (popcount(flags & (Utf::replace | Utf::throws)) > 1
                || popcount(flags & (IO::crlf | IO::lf | IO::striplf | IO::striptws | IO::stripws)) > 1)
            throw std::invalid_argument("Inconsistent file I/O flags");
        impl = std::make_shared<impl_type>();
        impl->file = file;
        impl->flags = flags;
        impl->enc = enc;
        impl->eol = eol;
        impl->lines = 0;
        if (enc.empty() || enc == "0")
            impl->enc = "utf-8";
        if ((flags & IO::standin) && (file.empty() || file.os_view() == dash))
            impl->handle.reset(stdin, null_delete);
        else
            impl->handle = shared_fopen(file, rb, ! (flags & IO::pretend));
        ++*this;
    }

    void FileReader::fixline() {
        if (impl->flags & IO::bom) {
            if (str_first_char(impl->line8) == byte_order_mark)
                impl->line8.erase(0, utf_begin(impl->line8).count());
            impl->flags &= ~ IO::bom;
        }
        if (impl->flags & IO::lf)
            impl->line8 += '\n';
        else if (impl->flags & IO::crlf)
            impl->line8 += "\r\n";
        else if (impl->flags & IO::striptws)
            impl->line8 = str_trim_right(impl->line8);
        else if (impl->flags & IO::stripws)
            impl->line8 = str_trim(impl->line8);
    }

    void FileReader::getline() {
        if (! impl)
            return;
        impl->line8.clear();
        size_t eolpos = npos, eolbytes = 0, eolsize = impl->eol.size();
        for (;;) {
            if (eolsize) {
                if (eolpos == npos)
                    eolpos = impl->rdbuf.find(impl->eol);
                if (eolpos != npos && (eolpos + eolsize < impl->rdbuf.size() || ! impl->is_ready()))
                    eolbytes = eolsize;
            } else {
                if (eolpos == npos)
                    eolpos = impl->rdbuf.find_first_of("\n\f\r");
                if (eolpos != npos) {
                    if (impl->rdbuf[eolpos] != '\r')
                        eolbytes = 1;
                    else if (impl->rdbuf.size() - eolpos >= 2 || ! impl->is_ready())
                        eolbytes = impl->rdbuf[eolpos + 1] == '\n' ? 2 : 1;
                }
            }
            if (eolbytes > 0 || ! impl->is_ready())
                break;
            getmore(256);
        }
        if (impl->rdbuf.empty()) {
            impl.reset();
            return;
        }
        std::string encoded(impl->rdbuf, 0, eolpos + eolbytes);
        impl->rdbuf.erase(0, eolpos + eolbytes);
        if (impl->flags & (IO::lf | IO::crlf | IO::striplf | IO::striptws | IO::stripws))
            encoded.resize(eolpos);
        import_string(encoded, impl->line8, impl->enc, impl->flags & (Utf::replace | Utf::throws));
        fixline();
        ++impl->lines;
    }

    void FileReader::getmore(size_t n) {
        if (! impl || n == 0)
            return;
        auto offset = impl->rdbuf.size();
        impl->rdbuf.resize(offset + n);
        auto rc = fread(&impl->rdbuf[offset], 1, n, impl->handle.get());
        auto err = errno;
        impl->rdbuf.resize(offset + rc);
        if (ferror(impl->handle.get()))
            throw std::system_error(err, std::generic_category(), quote(impl->file.name()));
    }

    // Class FileWriter

    struct FileWriter::impl_type {
        Ustring wrbuf;
        Path file;
        uint32_t flags;
        Ustring enc;
        SharedFile handle;
        std::shared_ptr<std::mutex> mutex;
    };

    void FileWriter::flush() {
        if (! impl)
            throw std::system_error(std::make_error_code(std::errc::bad_file_descriptor));
        if (fflush(impl->handle.get()) == EOF) {
            int err = errno;
            throw std::system_error(err, std::generic_category(), quote(impl->file.name()));
        }
    }

    void FileWriter::init(const Path& file, uint32_t flags, const Ustring& enc) {
        static constexpr NC dash[] = {NC('-'), NC(0)};
        static constexpr NC ab[] = {NC('a'), NC('b'), NC(0)};
        static constexpr NC wb[] = {NC('w'), NC('b'), NC(0)};
        static std::mutex stdout_mutex;
        static std::mutex stderr_mutex;
        if (popcount(flags & (Utf::replace | Utf::throws)) > 1
                || popcount(flags & (IO::append | IO::protect)) > 1
                || popcount(flags & (IO::autoline | IO::writeline)) > 1
                || popcount(flags & (IO::crlf | IO::lf)) > 1
                || popcount(flags & (IO::linebuf | IO::unbuf)) > 1
                || popcount(flags & (IO::standerr | IO::standout)) > 1)
            throw std::invalid_argument("Inconsistent file I/O flags");
        impl = std::make_shared<impl_type>();
        impl->file = file;
        impl->flags = flags;
        impl->enc = enc;
        if (enc.empty() || enc == "0")
            impl->enc = "utf-8";
        if ((flags & IO::standout) && (file.empty() || file.os_view() == dash))
            impl->handle.reset(stdout, null_delete);
        else if ((flags & IO::standerr) && (file.empty() || file.os_view() == dash))
            impl->handle.reset(stderr, null_delete);
        else if ((flags & IO::protect) && file.exists())
            throw std::system_error(std::make_error_code(std::errc::file_exists), quote(file.name()));
        else
            impl->handle = shared_fopen(file, flags & IO::append ? ab : wb, true);
        if (flags & IO::mutex) {
            if (impl->handle.get() == stdout)
                impl->mutex.reset(&stdout_mutex, null_delete);
            else if (impl->handle.get() == stderr)
                impl->mutex.reset(&stderr_mutex, null_delete);
            else
                impl->mutex = std::make_shared<std::mutex>();
        }
    }

    void FileWriter::fix_text(Ustring& str) const {
        if ((impl->flags & IO::writeline) || ((impl->flags & IO::autoline)
                && (str.empty() || ! char_is_line_break(str_last_char(str)))))
            str += '\n';
        if (impl->flags & (IO::lf | IO::crlf)) {
            Ustring brk = (impl->flags & IO::crlf) ? "\r\n"s : "\n"s;
            auto i = utf_begin(str);
            while (i.offset() < str.size()) {
                if (*i == '\r' && str[i.offset() + 1] == '\n')
                    i = str_insert_in(str, i, std::next(std::next(i)), brk).second;
                else if (char_is_line_break(*i))
                    i = str_insert_in(str, i, std::next(i), brk).second;
                else
                    ++i;
            }
        }
    }

    void FileWriter::write(Ustring str) {
        if (! impl)
            throw std::system_error(std::make_error_code(std::errc::bad_file_descriptor));
        fix_text(str);
        if (impl->flags & IO::linebuf) {
            str.insert(0, impl->wrbuf);
            impl->wrbuf.clear();
            auto b = utf_begin(str), e = utf_end(str), i = e;
            while (i != b) {
                --i;
                if (char_is_line_break(*i)) {
                    ++i;
                    break;
                }
            }
            if (i == b) {
                impl->wrbuf.swap(str);
            } else {
                impl->wrbuf = u_str(i, e);
                str.resize(i.offset());
            }
        }
        if (! str.empty()) {
            if (impl->flags & IO::bom) {
                if (str_first_char(str) != byte_order_mark)
                    str.insert(0, utf8_bom);
                impl->flags &= ~ IO::bom;
            }
            std::string encoded;
            export_string(str, encoded, impl->enc, impl->flags & (Utf::replace | Utf::throws));
            if (impl->mutex) {
                auto lock = make_lock(*impl->mutex);
                write_mbcs(encoded);
            } else {
                write_mbcs(encoded);
            }
        }
    }

    void FileWriter::write_mbcs(const std::string& str) {
        fwrite(str.data(), 1, str.size(), impl->handle.get());
        auto err = errno;
        if (ferror(impl->handle.get()))
            throw std::system_error(err, std::generic_category(), quote(impl->file.name()));
        if (impl->flags & (IO::linebuf | IO::unbuf))
            flush();
    }

}
