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

    }

    namespace UnicornDetail {

        void check_env(const NativeString& key) {
            if (key.empty() || key.find_first_of("=\0"_nat) != npos)
                throw std::invalid_argument("Invalid environment variable name");
        }

        void check_env(const NativeString& key, const NativeString& value) {
            check_env(key);
            if (value.find(NativeCharacter(0)) != npos)
                throw std::invalid_argument("Invalid environment variable value");
        }

        #if defined(PRI_TARGET_UNIX)

            void native_get_env(const NativeString& key, NativeString& value) {
                MutexLock lock(env_mutex);
                value = cstr(getenv(key.data()));
            }

            bool native_has_env(const NativeString& key) {
                MutexLock lock(env_mutex);
                return getenv(key.data()) != nullptr;
            }

            void native_set_env(const NativeString& key, const NativeString& value) {
                MutexLock lock(env_mutex);
                if (setenv(key.data(), value.data(), 1) == -1)
                    throw std::system_error(errno, std::generic_category(), "setenv()");
            }

            void native_unset_env(const NativeString& key) {
                MutexLock lock(env_mutex);
                if (unsetenv(key.data()) == -1)
                    throw std::system_error(errno, std::generic_category(), "unsetenv()");
            }

        #else

            void native_get_env(const NativeString& key, NativeString& value) {
                MutexLock lock(env_mutex);
                value = cstr(_wgetenv(key.data()));
            }

            bool native_has_env(const NativeString& key) {
                MutexLock lock(env_mutex);
                return _wgetenv(key.data()) != nullptr;
            }

            void native_set_env(const NativeString& key, const NativeString& value) {
                MutexLock lock(env_mutex);
                auto key_value = key + L'=' + value;
                if (_wputenv(key_value.data()) == -1)
                    throw std::system_error(errno, std::generic_category(), "_wputenv()");
            }

            void native_unset_env(const NativeString& key) {
                MutexLock lock(env_mutex);
                set_env(key, {});
            }

        #endif

    }

    // Class Environment

    Environment& Environment::operator=(const Environment& env) {
        map = env.map;
        deconstruct();
        return *this;
    }

    Environment& Environment::operator=(Environment&& env) noexcept {
        map = std::move(env.map);
        deconstruct();
        env.deconstruct();
        return *this;
    }

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
            str_partition_at(kv, key, value, "="_nat);
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
            temp_block += "="_nat;
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
