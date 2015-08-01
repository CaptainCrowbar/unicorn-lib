#include "unicorn/io.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/string.hpp"
#include "crow/thread.hpp"
#include <algorithm>
#include <cerrno>

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    // Exceptions

    u8string IOError::assemble(const char* msg, const u8string& file, int error) {
        auto s = Crow::cstr(msg);
        if (s.empty())
            s = "I/O error";
        if (! file.empty()) {
            s += "; file ";
            if (file.front() == '<' && file.back() == '>')
                s += file;
            else
                format_type(file, s, fx_quote);
        }
        if (error) {
            s += "; error ";
            s += Crow::dec(error);
            string details;
            import_string(system_message(error), details);
            if (! details.empty()) {
                s += "; ";
                s += details;
            }
        }
        return s;
    }

    // I/O implementation

    namespace {

        using SharedFile = std::shared_ptr<FILE>;

        void checked_fclose(FILE* f) {
            if (f)
                fclose(f);
        }

        SharedFile shared_fopen(const NativeString& file, const NativeString& mode, bool check) {
            FILE* f =
                #if defined(CROW_TARGET_UNIX)
                    fopen
                #else
                    _wfopen
                #endif
                (file.data(), mode.data());
            if (f || ! check)
                return {f, checked_fclose};
            else if (mode == "rb"_nat)
                throw ReadError(file, errno);
            else
                throw WriteError(file, errno);
        }

    }

    // Simple file I/O

    namespace UnicornDetail {

        void native_load_file(const NativeString& file, string& dst, Crow::Flagset flags) {
            static constexpr size_t bufsize = 65536;
            static const auto dashfile = "-"_nat;
            dst.clear();
            flags.allow(io_stdin | io_nofail, "load file");
            SharedFile handle;
            if (flags.get(io_stdin) && (file.empty() || file == dashfile))
                handle.reset(stdin, Crow::do_nothing);
            else
                handle = shared_fopen(file, "rb"_nat, ! flags.get(io_nofail));
            if (! handle)
                return;
            string buf(bufsize, 0);
            while (! feof(handle.get())) {
                auto rc = fread(&buf[0], 1, bufsize, handle.get());
                auto err = errno;
                if (ferror(handle.get()))
                    throw ReadError(file, err);
                dst.append(buf, 0, rc);
            }
        }

        void native_save_file(const NativeString& file, const void* ptr, size_t n, Crow::Flagset flags) {
            static const auto dashfile = "-"_nat;
            flags.allow(io_append | io_stderr | io_stdout, "save file");
            flags.exclusive(io_stderr | io_stdout, "save file");
            SharedFile handle;
            if (flags.get(io_stdout) && (file.empty() || file == dashfile))
                handle.reset(stdout, Crow::do_nothing);
            else if (flags.get(io_stderr) && (file.empty() || file == dashfile))
                handle.reset(stderr, Crow::do_nothing);
            else
                handle = shared_fopen(file, flags.get(io_append) ? "ab"_nat : "wb"_nat, true);
            fwrite(ptr, 1, n, handle.get());
            auto err = errno;
            if (ferror(handle.get()))
                throw WriteError(file, err);
        }

    }

    // Class FileReader

    struct FileReader::impl_type {
        u8string line8;
        string rdbuf;
        NativeString name;
        Crow::Flagset flags;
        u8string enc;
        u8string eol;
        SharedFile handle;
        bool is_ready() noexcept {
            return handle.get() && ! ferror(handle.get()) && ! feof(handle.get());
        }
    };

    const u8string& FileReader::operator*() const noexcept {
        static const u8string dummy;
        return impl ? impl->line8 : dummy;
    }

    FileReader& FileReader::operator++() {
        do getline();
            while (impl && impl->flags.get(io_notempty)
                && std::all_of(utf_begin(impl->line8), utf_end(impl->line8), char_is_line_break));
        return *this;
    }

    void FileReader::init(const NativeString& file, Crow::Flagset flags, const u8string& enc, const u8string& eol) {
        static const auto dashfile = "-"_nat;
        flags.allow(err_replace | err_throw | io_bom | io_crlf | io_lf
            | io_nofail | io_notempty | io_stdin | io_striplf | io_striptws
            | io_stripws, "file input");
        flags.exclusive(err_replace | err_throw, "file input");
        flags.exclusive(io_crlf | io_lf | io_striplf | io_striptws | io_stripws, "file input");
        impl = std::make_shared<impl_type>();
        impl->name = file;
        impl->flags = flags;
        impl->enc = enc;
        impl->eol = eol;
        if (enc.empty() || enc == "0")
            impl->enc = "utf-8";
        if (flags.get(io_stdin) && (file.empty() || file == dashfile))
            impl->handle.reset(stdin, Crow::do_nothing);
        else
            impl->handle = shared_fopen(file, "rb"_nat, ! flags.get(io_nofail));
        ++*this;
    }

    void FileReader::fixline() {
        if (impl->flags.get(io_bom)) {
            if (str_first_char(impl->line8) == byte_order_mark)
                impl->line8.erase(0, utf_begin(impl->line8).count());
            impl->flags.set(io_bom, false);
        }
        if (impl->flags.get(io_lf))
            impl->line8 += '\n';
        else if (impl->flags.get(io_crlf))
            impl->line8 += "\r\n";
        else if (impl->flags.get(io_striptws))
            impl->line8 = str_trim_right(impl->line8);
        else if (impl->flags.get(io_stripws))
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
        string encoded(impl->rdbuf, 0, eolpos + eolbytes);
        impl->rdbuf.erase(0, eolpos + eolbytes);
        if (impl->flags.get(io_lf | io_crlf | io_striplf | io_striptws | io_stripws))
            encoded.resize(eolpos);
        import_string(encoded, impl->line8, impl->enc, impl->flags & (err_replace | err_throw));
        fixline();
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
            throw ReadError(impl->name, err);
    }

    // Class FileWriter

    struct FileWriter::impl_type {
        u8string wrbuf;
        NativeString name;
        Crow::Flagset flags;
        u8string enc;
        SharedFile handle;
        std::shared_ptr<Crow::Mutex> mutex;
    };

    void FileWriter::flush() {
        if (! impl)
            throw WriteError();
        if (fflush(impl->handle.get()) == EOF)
            throw WriteError(impl->name, errno);
    }

    void FileWriter::init(const NativeString& file, Crow::Flagset flags, const u8string& enc) {
        static const NativeString dashfile{NativeCharacter('-')};
        static Crow::Mutex stdout_mutex;
        static Crow::Mutex stderr_mutex;
        flags.allow(err_replace | err_throw | io_append | io_autoline | io_bom
            | io_crlf | io_lf | io_linebuf | io_mutex | io_stderr
            | io_stdout | io_unbuf | io_writeline, "file output");
        flags.exclusive(err_replace | err_throw, "file output");
        flags.exclusive(io_autoline | io_writeline, "file output");
        flags.exclusive(io_crlf | io_lf, "file output");
        flags.exclusive(io_linebuf | io_unbuf, "file output");
        flags.exclusive(io_stderr | io_stdout, "file output");
        impl = std::make_shared<impl_type>();
        impl->name = file;
        impl->flags = flags;
        impl->enc = enc;
        if (enc.empty() || enc == "0")
            impl->enc = "utf-8";
        if (flags.get(io_stdout) && (file.empty() || file == dashfile))
            impl->handle.reset(stdout, Crow::do_nothing);
        else if (flags.get(io_stderr) && (file.empty() || file == dashfile))
            impl->handle.reset(stderr, Crow::do_nothing);
        else
            impl->handle = shared_fopen(file, flags.get(io_append) ? "ab"_nat : "wb"_nat, true);
        if (flags.get(io_mutex)) {
            if (impl->handle.get() == stdout)
                impl->mutex.reset(&stdout_mutex, Crow::do_nothing);
            else if (impl->handle.get() == stderr)
                impl->mutex.reset(&stderr_mutex, Crow::do_nothing);
            else
                impl->mutex = std::make_shared<Crow::Mutex>();
        }
    }

    void FileWriter::fixtext(u8string& str) const {
        if (impl->flags.get(io_writeline) || (impl->flags.get(io_autoline)
                && (str.empty() || ! char_is_line_break(str_last_char(str)))))
            str += '\n';
        if (impl->flags.get(io_lf | io_crlf)) {
            u8string brk = impl->flags.get(io_crlf) ? "\r\n"s : "\n"s;
            auto i = utf_begin(str);
            while (i.offset() < str.size()) {
                if (*i == '\r' && str[i.offset() + 1] == '\n')
                    i = str_change_in(str, i, std::next(std::next(i)), brk).second;
                else if (char_is_line_break(*i))
                    i = str_change_in(str, i, std::next(i), brk).second;
                else
                    ++i;
            }
        }
    }

    void FileWriter::write(u8string str) {
        if (! impl)
            throw WriteError();
        fixtext(str);
        if (impl->flags.get(io_linebuf)) {
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
            if (impl->flags.get(io_bom)) {
                if (str_first_char(str) != byte_order_mark)
                    str.insert(0, utf8_bom);
                impl->flags.set(io_bom, false);
            }
            string encoded;
            export_string(str, encoded, impl->enc, impl->flags & (err_replace | err_throw));
            if (impl->mutex) {
                Crow::MutexLock lock(*impl->mutex);
                writembcs(encoded);
            } else {
                writembcs(encoded);
            }
        }
    }

    void FileWriter::writembcs(const string& str) {
        fwrite(str.data(), 1, str.size(), impl->handle.get());
        auto err = errno;
        if (ferror(impl->handle.get()))
            throw WriteError(impl->name, err);
        if (impl->flags.get(io_linebuf | io_unbuf))
            flush();
    }

}
