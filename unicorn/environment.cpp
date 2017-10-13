#include "unicorn/environment.hpp"
#include "unicorn/string.hpp"
#include "rs-core/thread.hpp"
#include <cerrno>
#include <cstdlib>
#include <stdexcept>
#include <system_error>

#ifdef _XOPEN_SOURCE
    extern char** environ;
#else
    extern wchar_t** _wenviron;
#endif

namespace RS::Unicorn {

    namespace {

        Mutex env_mutex;

        void check_env(const NativeString& name) {
            static const NativeString not_allowed{'\0','='};
            if (name.empty() || name.find_first_of(not_allowed) != npos)
                throw std::invalid_argument("Invalid environment variable name");
        }

        void check_env(const NativeString& name, const NativeString& value) {
            check_env(name);
            if (value.find(NativeCharacter(0)) != npos)
                throw std::invalid_argument("Invalid environment variable value");
        }

        template <typename C>
        std::basic_string<C> expand_posix_var(const std::basic_string<C>& src, size_t& ofs, Environment* env) {
            using S = std::basic_string<C>;
            static constexpr C c_dollar = C('$');
            static constexpr C c_lbrace = C('{');
            static constexpr C c_rbrace = C('}');
            static const S s_dollar = {c_dollar};
            static const S s_dollar_lbrace = {c_dollar, c_lbrace};
            if (src.size() - ofs == 1) {
                ++ofs;
                return s_dollar;
            }
            S name;
            if (src[ofs + 1] == c_dollar) {
                ofs += 2;
                return s_dollar;
            } else if (src[ofs + 1] == c_lbrace) {
                size_t end = src.find(c_rbrace, ofs + 2);
                if (end == npos) {
                    ofs += 2;
                    return s_dollar_lbrace;
                }
                name = src.substr(ofs + 2, end - ofs - 2);
                ofs = end + 1;
            } else {
                size_t end = ofs + 1;
                while (end < src.size() && char_is_alphanumeric_w(src[end]))
                    ++end;
                if (end - ofs == 1) {
                    ++ofs;
                    return s_dollar;
                }
                name = src.substr(ofs + 1, end - ofs - 1);
                ofs = end;
            }
            if (env)
                return env->get(name);
            else
                return get_env(name);
        }

        template <typename C>
        std::basic_string<C> expand_windows_var(const std::basic_string<C>& src, size_t& ofs, Environment* env) {
            using S = std::basic_string<C>;
            static constexpr C c_percent = C('%');
            static const S s_percent = {c_percent};
            if (src.size() - ofs == 1) {
                ++ofs;
                return s_percent;
            }
            S name;
            size_t end = ofs;
            if (src[ofs + 1] == c_percent) {
                ofs += 2;
                return s_percent;
            } else {
                end = src.find(c_percent, ofs + 1);
                if (end == npos) {
                    ++ofs;
                    return s_percent;
                }
                name = src.substr(ofs + 1, end - ofs - 1);
                ofs = end + 1;
            }
            if (env)
                return env->get(name);
            else
                return get_env(name);
        }

        template <typename C>
        std::basic_string<C> do_expand_env(const std::basic_string<C>& src, uint32_t flags, Environment* env) {
            using S = std::basic_string<C>;
            static constexpr C c_dollar = C('$');
            static constexpr C c_percent = C('%');
            S delims;
            if (flags & Environment::posix)
                delims += c_dollar;
            if (flags & Environment::windows)
                delims += c_percent;
            if (delims.empty() || src.empty())
                return src;
            S dst;
            size_t i = 0;
            while (i < src.size()) {
                size_t j = src.find_first_of(delims, i);
                if (j == npos) {
                    dst.append(src, i, npos);
                    break;
                }
                if (j > i)
                    dst.append(src, i, j - i);
                if (src[j] == c_dollar)
                    dst += expand_posix_var(src, j, env);
                else
                    dst += expand_windows_var(src, j, env);
                i = j;
            }
            return dst;
        }

    }

    // Class Environment

    Environment::Environment(bool from_process) {
        if (from_process)
            load();
    }

    Environment::Environment(const Environment& env):
    map(env.map), block(), index() {}

    Environment::Environment(Environment&& env) noexcept:
    map(move(env.map)), block(), index() {
        env.deconstruct();
    }

    Environment& Environment::operator=(const Environment& env) {
        map = env.map;
        deconstruct();
        return *this;
    }

    Environment& Environment::operator=(Environment&& env) noexcept {
        map = move(env.map);
        deconstruct();
        env.deconstruct();
        return *this;
    }

    NativeString Environment::expand(const NativeString& src, uint32_t flags) {
        return do_expand_env(src, flags, this);
    }

    NativeString Environment::get(const NativeString& name) {
        auto it = map.find(name);
        return it == map.end() ? NativeString() : it->second;
    }

    bool Environment::has(const NativeString& name) {
        return map.count(name) != 0;
    }

    void Environment::set(const NativeString& name, const NativeString& value) {
        deconstruct();
        map[name] = value;
    }

    void Environment::unset(const NativeString& name) {
        deconstruct();
        map.erase(name);
    }

    #ifndef _XOPEN_SOURCE

        U8string Environment::expand(const U8string& src, uint32_t flags) {
            return do_expand_env(src, flags, this);
        }

        U8string Environment::get(const U8string& name) {
            return to_utf8(get(to_wstring(name)));
        }

        bool Environment::has(const U8string& name) {
            return has(to_wstring(name));
        }

        void Environment::set(const U8string& name, const U8string& value) {
            return set(to_wstring(name), to_wstring(value));
        }

        void Environment::unset(const U8string& name) {
            unset(to_wstring(name));
        }

    #endif

    void Environment::load() {
        deconstruct();
        auto ptr =
            #ifdef _XOPEN_SOURCE
                environ;
            #else
                _wenviron;
            #endif
        if (ptr == nullptr) {
            map.clear();
            return;
        }
        string_map env;
        NativeString key, value, kv;
        for (; *ptr != nullptr; ++ptr) {
            kv = *ptr;
            size_t cut = kv.find(NativeCharacter('='));
            if (cut == npos) {
                key = kv;
                value.clear();
            } else {
                key = kv.substr(0, cut);
                value = kv.substr(cut + 1, npos);
            }
            env[key] = value;
        }
        map.swap(env);
    }

    void Environment::reconstruct() {
        if (! block.empty())
            return;
        NativeString temp_block;
        std::vector<size_t> offsets;
        for (auto& kv: map) {
            offsets.push_back(temp_block.size());
            temp_block += kv.first;
            temp_block += NativeCharacter('=');
            temp_block += kv.second;
            temp_block += NativeCharacter(0);
        }
        temp_block += NativeCharacter(0);
        index.resize(offsets.size() + 1);
        block.swap(temp_block);
        for (size_t i = 0; i < offsets.size(); ++i)
            index[i] = &block[0] + offsets[i];
    }

    // Functions

    #ifdef _XOPEN_SOURCE

        std::string expand_env(const std::string& src, uint32_t flags) {
            return do_expand_env(src, flags, nullptr);
        }

        std::string get_env(const std::string& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            return cstr(getenv(name.data()));
        }

        bool has_env(const std::string& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            return getenv(name.data()) != nullptr;
        }

        void set_env(const std::string& name, const std::string& value) {
            check_env(name, value);
            MutexLock lock(env_mutex);
            if (setenv(name.data(), value.data(), 1) == -1)
                throw std::system_error(errno, std::generic_category(), "setenv()");
        }

        void unset_env(const std::string& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            if (unsetenv(name.data()) == -1)
                throw std::system_error(errno, std::generic_category(), "unsetenv()");
        }

    #else

        std::wstring expand_env(const std::wstring& src, uint32_t flags) {
            return do_expand_env(src, flags, nullptr);
        }

        std::wstring get_env(const std::wstring& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            return cstr(_wgetenv(name.data()));
        }

        bool has_env(const std::wstring& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            return _wgetenv(name.data()) != nullptr;
        }

        void set_env(const std::wstring& name, const std::wstring& value) {
            check_env(name, value);
            MutexLock lock(env_mutex);
            auto key_value = name + L'=' + value;
            if (_wputenv(key_value.data()) == -1)
                throw std::system_error(errno, std::generic_category(), "_wputenv()");
        }

        void unset_env(const std::wstring& name) {
            check_env(name);
            MutexLock lock(env_mutex);
            set_env(name, {});
        }

    #endif

}
