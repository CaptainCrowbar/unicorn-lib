#pragma once

#include "unicorn/core.hpp"
#include <cstddef>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Unicorn {

    namespace Json {

        class Exception:
        public std::runtime_error {
        public:
            explicit Exception(u8string message, size_t pos = npos):
                std::runtime_error(assemble(message, pos)), epos(pos) {}
            size_t pos() const noexcept { return epos; }
        private:
            size_t epos;
            static u8string assemble(const u8string& message, size_t pos);
        };

        class BadJson: public Exception { public: explicit BadJson(size_t pos); };
        class EndOfFile: public Exception { public: explicit EndOfFile(size_t pos); };
        class WrongType: public Exception { public: WrongType(); };

        enum ElementType {
            null,
            boolean,
            number,
            string,
            array,
            object,
        };

        class Element;
        using Array = std::vector<Element>;
        using Object = std::map<u8string, Element>;

        class Element:
        public EqualityComparable<Element> {
        public:
            Element() noexcept: etype(Json::null) {}
            Element(std::nullptr_t) noexcept: etype(Json::null) {}
            Element(bool value) noexcept:
                etype(Json::boolean) { rep.boolean = value; }
            template <typename T> Element(const T& value,
                typename std::enable_if_t<std::is_arithmetic<T>::value>* = nullptr) noexcept:
                etype(Json::number) { rep.number = value; }
            Element(const u8string& value):
                etype(Json::string) { rep.string = new u8string(value); }
            Element(const char* value):
                etype(Json::string) { rep.string = value ? new u8string(value) : new u8string; }
            Element(const Array& value):
                etype(Json::array) { rep.array = new Array(value); }
            template <typename... Args> Element(const Args&... args):
                etype(Json::array) { rep.array = new Array{args...}; }
            Element(const Object& value):
                etype(Json::object) { rep.object = new Object(value); }
            Element(const Element& e);
            Element(Element&& e) noexcept;
            ~Element() noexcept { make_null(); }
            Element& operator=(const Element& e);
            Element& operator=(Element&& e) noexcept;
            Element& operator[](size_t key);
            const Element& operator[](size_t key) const;
            Element& operator[](const u8string& key);
            const Element& operator[](const u8string& key) const;
            bool& boolean() { check_type(Json::boolean); return rep.boolean; }
            bool boolean() const { check_type(Json::boolean); return rep.boolean; }
            double& number() { check_type(Json::number); return rep.number; }
            double number() const { check_type(Json::number); return rep.number; }
            u8string& string() { check_type(Json::string); return *rep.string; }
            const u8string& string() const { check_type(Json::string); return *rep.string; }
            Array& array() { check_type(Json::array); return *rep.array; }
            const Array& array() const { check_type(Json::array); return *rep.array; }
            Object& object() { check_type(Json::object); return *rep.object; }
            const Object& object() const { check_type(Json::object); return *rep.object; }
            void layout(std::ostream& out, size_t max_array = 0) const;
            u8string str() const { u8string s; write(s); return s; }
            ElementType type() const noexcept { return etype; }
            void write(u8string& dst) const;
            static Element read(const u8string& src);
            static Element read(const u8string& src, size_t& pos);
            friend bool operator==(const Element& lhs, const Element& rhs) noexcept;
        private:
            using rep_type = union {
                bool boolean;
                double number;
                u8string* string;
                Array* array;
                Object* object;
            };
            static constexpr size_t rep_size = sizeof(rep_type);
            ElementType etype;
            rep_type rep;
            void check_type(int type) const { if (etype != type) throw WrongType(); }
            void make_null() noexcept;
            void read_null(const u8string& src, size_t& pos);
            void read_boolean(const u8string& src, size_t& pos);
            void read_number(const u8string& src, size_t& pos);
            void read_string(const u8string& src, size_t& pos);
            void read_array(const u8string& src, size_t& pos);
            void read_object(const u8string& src, size_t& pos);
            void write_null(u8string& dst) const;
            void write_boolean(u8string& dst) const;
            void write_number(u8string& dst) const;
            void write_string(u8string& dst) const;
            void write_array(u8string& dst) const;
            void write_object(u8string& dst) const;
        };

        inline std::ostream& operator<<(std::ostream& out, const Element& e) {
            u8string s;
            e.write(s);
            return out << s;
        }

    }

}
