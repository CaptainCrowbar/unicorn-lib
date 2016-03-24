#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <iterator>
#include <map>
#include <string>
#include <utility>

namespace Unicorn {

    namespace UnicornDetail {

        void check_env(const NativeString& key);
        void check_env(const NativeString& key, const NativeString& value);
        void native_get_env(const NativeString& key, NativeString& value);
        bool native_has_env(const NativeString& key);
        void native_set_env(const NativeString& key, const NativeString& value);
        void native_unset_env(const NativeString& key);

    }

    // Functions

    template <typename C>
    basic_string<C> get_env(const basic_string<C>& name, uint32_t flags = 0) {
        using namespace UnicornDetail;
        NativeString nkey = to_native(name, flags), nvalue;
        check_env(nkey);
        native_get_env(nkey, nvalue);
        return recode<C>(nvalue, flags);
    }

    template <typename C>
    basic_string<C> get_env(const C* name, uint32_t flags = 0) {
        return get_env(cstr(name), flags);
    }

    template <typename C>
    bool has_env(const basic_string<C>& name, uint32_t flags = 0) {
        using namespace UnicornDetail;
        NativeString nkey = to_native(name, flags);
        check_env(nkey);
        return native_has_env(nkey);
    }

    template <typename C>
    bool has_env(const C* name, uint32_t flags = 0) {
        return has_env(cstr(name), flags);
    }

    template <typename C>
    void set_env(const basic_string<C>& name, const basic_string<C>& value, uint32_t flags = 0) {
        using namespace UnicornDetail;
        NativeString nkey = to_native(name, flags), nvalue = to_native(value, flags);
        check_env(nkey, nvalue);
        native_set_env(nkey, nvalue);
    }

    template <typename C>
    void set_env(const basic_string<C>& name, const C* value, uint32_t flags = 0) {
        set_env(name, cstr(value), flags);
    }

    template <typename C>
    void set_env(const C* name, const basic_string<C>& value, uint32_t flags = 0) {
        set_env(cstr(name), value, flags);
    }

    template <typename C>
    void set_env(const C* name, const C* value, uint32_t flags = 0) {
        set_env(cstr(name), cstr(value), flags);
    }

    template <typename C>
    void unset_env(const basic_string<C>& name, uint32_t flags = 0) {
        using namespace UnicornDetail;
        NativeString nkey = to_native(name, flags);
        check_env(nkey);
        native_unset_env(nkey);
    }

    template <typename C>
    void unset_env(const C* name, uint32_t flags = 0) {
        unset_env(cstr(name), flags);
    }

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
        explicit Environment(bool from_process) { if (from_process) load(); }
        Environment(const Environment& env): map(env.map), block(), index() {}
        Environment(Environment&& env) noexcept: map(move(env.map)), block(), index() { env.deconstruct(); }
        ~Environment() noexcept = default;
        Environment& operator=(const Environment& env);
        Environment& operator=(Environment&& env) noexcept;
        template <typename C> basic_string<C> operator[](const basic_string<C>& name) { return get(name); }
        template <typename C> basic_string<C> operator[](const C* name) { return get(cstr(name)); }
        template <typename C> basic_string<C> get(const basic_string<C>& name, uint32_t flags = 0) {
            auto it = map.find(to_native(name, flags));
            return it == map.end() ? basic_string<C>() : recode<C>(it->second, flags);
        }
        template <typename C> basic_string<C> get(const C* name, uint32_t flags = 0) { return get(cstr(name), flags); }
        template <typename C> bool has(const basic_string<C>& name, uint32_t flags = 0) { return map.count(to_native(name, flags)) != 0; }
        template <typename C> bool has(const C* name, uint32_t flags = 0) { return has(cstr(name), flags); }
        template <typename C> void set(const basic_string<C>& name, const basic_string<C>& value, uint32_t flags = 0) {
            NativeString nkey = to_native(name, flags), nvalue = to_native(value, flags);
            deconstruct();
            map[nkey] = nvalue;
        }
        template <typename C> void set(const basic_string<C>& name, const C* value, uint32_t flags = 0) { set(name, cstr(value), flags); }
        template <typename C> void set(const C* name, const basic_string<C>& value, uint32_t flags = 0) { set(cstr(name), value, flags); }
        template <typename C> void set(const C* name, const C* value, uint32_t flags = 0) { set(cstr(name), cstr(value), flags); }
        template <typename C> void unset(const basic_string<C>& name, uint32_t flags = 0) {
            deconstruct();
            map.erase(to_native(name, flags));
        }
        template <typename C> void unset(const C* name, uint32_t flags = 0) { unset(cstr(name), flags); }
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
