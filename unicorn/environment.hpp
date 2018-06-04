#pragma once

#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace RS::Unicorn {

    // Class Environment

    class Environment {
    private:
        using ptr_list = std::vector<NativeCharacter*>;
        using string_map = std::map<NativeString, NativeString>;
        using string_pair = std::pair<NativeString, NativeString>;
    public:
        static constexpr uint32_t posix = 1;
        static constexpr uint32_t windows = 2;
        #ifdef _XOPEN_SOURCE
            static constexpr uint32_t native = posix;
        #else
            static constexpr uint32_t native = windows;
        #endif
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
        NativeString expand(const NativeString& src, uint32_t flags = native);
        NativeString get(const NativeString& name);
        bool has(const NativeString& name);
        void set(const NativeString& name, const NativeString& value);
        void unset(const NativeString& name);
        #ifndef _XOPEN_SOURCE
            Ustring operator[](const Ustring& name) { return get(name); }
            Ustring expand(const Ustring& src, uint32_t flags = native);
            Ustring get(const Ustring& name);
            bool has(const Ustring& name);
            void set(const Ustring& name, const Ustring& value);
            void unset(const Ustring& name);
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

    // Functions

    #ifdef _XOPEN_SOURCE

        std::string expand_env(const std::string& src, uint32_t flags = Environment::native);
        std::string get_env(const std::string& name);
        bool has_env(const std::string& name);
        void set_env(const std::string& name, const std::string& value);
        void unset_env(const std::string& name);

    #else

        std::wstring expand_env(const std::wstring& src, uint32_t flags = Environment::native);
        std::wstring get_env(const std::wstring& name);
        bool has_env(const std::wstring& name);
        void set_env(const std::wstring& name, const std::wstring& value);
        void unset_env(const std::wstring& name);

        inline Ustring expand_env(const Ustring& src, uint32_t flags = Environment::native) {
            return to_utf8(expand_env(to_wstring(src, Utf::replace), flags), Utf::replace);
        }

        inline Ustring get_env(const Ustring& name) {
            return to_utf8(get_env(to_wstring(name, Utf::replace)), Utf::replace);
        }

        inline bool has_env(const Ustring& name) {
            return has_env(to_wstring(name, Utf::replace));
        }

        inline void set_env(const Ustring& name, const Ustring& value) {
            set_env(to_wstring(name, Utf::replace), to_wstring(value, Utf::replace));
        }

        inline void unset_env(const Ustring& name) {
            unset_env(to_wstring(name, Utf::replace));
        }

    #endif

}
