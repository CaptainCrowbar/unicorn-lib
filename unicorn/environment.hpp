#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <iterator>
#include <map>
#include <string>
#include <utility>

namespace Unicorn {


    // Functions

    #if defined(PRI_TARGET_UNIX)

        string get_env(const string& key);
        bool has_env(const string& key);
        void set_env(const string& key, const string& value);
        void unset_env(const string& key);

    #else

        wstring get_env(const wstring& key);
        bool has_env(const wstring& key);
        void set_env(const wstring& key, const wstring& value);
        void unset_env(const wstring& key);

        inline u8string get_env(const u8string& key) {
            return to_utf8(get_env(to_wstring(name, err_replace)), err_replace);
        }

        inline bool has_env(const u8string& key) {
            return has_env(to_wstring(name, err_replace));
        }

        inline void set_env(const u8string& key, const u8string& value) {
            set_env(to_wstring(name, err_replace), to_wstring(value, err_replace));
        }

        inline void unset_env(const u8string& key) {
            unset_env(to_wstring(name, err_replace));
        }

    #endif

    // Class Environment

    class Environment {
    private:
        using ptr_list = vector<NativeCharacter*>;
        using string_map = std::map<NativeString, NativeString>;
        using string_pair = pair<NativeString, NativeString>;
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
        NativeString get(const NativeString& name);
        bool has(const NativeString& name);
        void set(const NativeString& name, const NativeString& value);
        void unset(const NativeString& name);
        #if defined(PRI_TARGET_WINDOWS)
            u8string operator[](const u8string& name) { return get(name); }
            u8string get(const u8string& name);
            bool has(const u8string& name);
            void set(const u8string& name, const u8string& value);
            void unset(const u8string& name);
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
