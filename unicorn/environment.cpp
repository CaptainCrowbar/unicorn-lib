#include "unicorn/environment.hpp"
#include "unicorn/string.hpp"
#include <cerrno>
#include <cstdlib>
#include <system_error>

#if defined(PRI_TARGET_UNIX)
    extern char** environ;
#else
    extern wchar_t** _wenviron;
#endif

using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        Mutex env_mutex;

        void check_env(const NativeString& key) {
            static const NativeString not_allowed{'\0','='};
            if (key.empty() || key.find_first_of(not_allowed) != npos)
                throw std::invalid_argument("Invalid environment variable name");
        }

        void check_env(const NativeString& key, const NativeString& value) {
            check_env(key);
            if (value.find(NativeCharacter(0)) != npos)
                throw std::invalid_argument("Invalid environment variable value");
        }

    }

    #if defined(PRI_TARGET_UNIX)

        string get_env(const string& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            return cstr(getenv(key.data()));
        }

        bool has_env(const string& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            return getenv(key.data()) != nullptr;
        }

        void set_env(const string& key, const string& value) {
            check_env(key, value);
            MutexLock lock(env_mutex);
            if (setenv(key.data(), value.data(), 1) == -1)
                throw std::system_error(errno, std::generic_category(), "setenv()");
        }

        void unset_env(const string& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            if (unsetenv(key.data()) == -1)
                throw std::system_error(errno, std::generic_category(), "unsetenv()");
        }

    #else

        wstring get_env(const wstring& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            return cstr(_wgetenv(key.data()));
        }

        bool has_env(const wstring& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            return _wgetenv(key.data()) != nullptr;
        }

        void set_env(const wstring& key, const wstring& value) {
            check_env(key, value);
            MutexLock lock(env_mutex);
            auto key_value = key + L'=' + value;
            if (_wputenv(key_value.data()) == -1)
                throw std::system_error(errno, std::generic_category(), "_wputenv()");
        }

        void unset_env(const wstring& key) {
            check_env(key);
            MutexLock lock(env_mutex);
            set_env(key, {});
        }

    #endif

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

    #if defined(PRI_TARGET_WINDOWS)

        u8string Environment::get(const u8string& name) {
            return to_utf8(get(to_wstring(name)));
        }

        bool Environment::has(const u8string& name) {
            return has(to_wstring(name));
        }

        void Environment::set(const u8string& name, const u8string& value) {
            return set(to_wstring(name), to_wstring(value));
        }

        void Environment::unset(const u8string& name) {
            unset(to_wstring(name));
        }

    #endif

    void Environment::load() {
        deconstruct();
        auto ptr =
            #if defined(PRI_TARGET_UNIX)
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
            str_partition_at(kv, key, value, PRI_CSTR("=", NativeCharacter));
            env[key] = value;
        }
        map.swap(env);
    }

    void Environment::reconstruct() {
        if (! block.empty())
            return;
        NativeString temp_block;
        vector<size_t> offsets;
        for (auto& kv: map) {
            offsets.push_back(temp_block.size());
            temp_block += kv.first;
            temp_block += PRI_CHAR('=', NativeCharacter);
            temp_block += kv.second;
            temp_block += NativeCharacter(0);
        }
        temp_block += NativeCharacter(0);
        index.resize(offsets.size() + 1);
        block.swap(temp_block);
        for (size_t i = 0; i < offsets.size(); ++i)
            index[i] = &block[0] + offsets[i];
    }

}
