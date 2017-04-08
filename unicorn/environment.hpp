#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Unicorn {

    // Constants

    constexpr uint32_t posix_env = 1;
    constexpr uint32_t windows_env = 2;

    #ifdef _XOPEN_SOURCE
        constexpr uint32_t native_env = posix_env;
    #else
        constexpr uint32_t native_env = windows_env;
    #endif

    // Functions

    #ifdef _XOPEN_SOURCE

        std::string expand_env(const std::string& src, uint32_t flags = native_env);
        std::string get_env(const std::string& name);
        bool has_env(const std::string& name);
        void set_env(const std::string& name, const std::string& value);
        void unset_env(const std::string& name);

    #else

        std::wstring expand_env(const std::wstring& src, uint32_t flags = native_env);
        std::wstring get_env(const std::wstring& name);
        bool has_env(const std::wstring& name);
        void set_env(const std::wstring& name, const std::wstring& value);
        void unset_env(const std::wstring& name);

        inline U8string expand_env(const U8string& src, uint32_t flags = native_env) {
            return to_utf8(expand_env(to_wstring(src, err_replace), flags), err_replace);
        }

        inline U8string get_env(const U8string& name) {
            return to_utf8(get_env(to_wstring(name, err_replace)), err_replace);
        }

        inline bool has_env(const U8string& name) {
            return has_env(to_wstring(name, err_replace));
        }

        inline void set_env(const U8string& name, const U8string& value) {
            set_env(to_wstring(name, err_replace), to_wstring(value, err_replace));
        }

        inline void unset_env(const U8string& name) {
            unset_env(to_wstring(name, err_replace));
        }

    #endif

    // Class Environment

    class Environment {
    private:
        using ptr_list = std::vector<NativeCharacter*>;
        using string_map = std::map<NativeString, NativeString>;
        using string_pair = std::pair<NativeString, NativeString>;
    public:
        class iterator:
        public ForwardIterator<iterator, const string_pair> {
        public:
            iterator() = default;
            const string_pair& operator*() const { return reinterpret_cast<const string_pair&>(*iter); }
            iterator& operator++() { ++iter; return *this; }
            bool operator==(const iterator& rhs) const noexcept { return iter == rhs.iter; }
        private:
            friend class Environment;
            string_map::const_iterator iter;
            explicit iterator(string_map::const_iterator i): iter(i) {}
        };
        Environment() = default;
        explicit Environment(bool from_process);
        Environment(const Environment& env);
        Environment(Environment&& env) noexcept;
        ~Environment() = default;
        Environment& operator=(const Environment& env);
        Environment& operator=(Environment&& env) noexcept;
        NativeString operator[](const NativeString& name) { return get(name); }
        NativeString expand(const NativeString& src, uint32_t flags = native_env);
        NativeString get(const NativeString& name);
        bool has(const NativeString& name);
        void set(const NativeString& name, const NativeString& value);
        void unset(const NativeString& name);
        #ifndef _XOPEN_SOURCE
            U8string operator[](const U8string& name) { return get(name); }
            U8string expand(const U8string& src, uint32_t flags = native_env);
            U8string get(const U8string& name);
            bool has(const U8string& name);
            void set(const U8string& name, const U8string& value);
            void unset(const U8string& name);
        #endif
        iterator begin() const { return iterator(map.begin()); }
        iterator end() const { return iterator(map.end()); }
        void clear() { map.clear(); block.clear(), index.clear(); }
        bool empty() const noexcept { return map.empty(); }
        void load();
        NativeCharacter** ptr() { reconstruct(); return index.data(); }
        size_t size() const noexcept { return map.size(); }
    private:
        string_map map;
        NativeString block;
        ptr_list index;
        void deconstruct() { block.clear(); index.clear(); }
        void reconstruct();
    };

}
