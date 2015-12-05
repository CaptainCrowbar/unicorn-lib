Title: Unicorn Library: JSON
CSS: style.css

# [Unicorn Library](index.html): JSON #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/json.hpp"` ####

[TODO: DOCUMENTATION]

## Contents ##

* [Exceptions][]
* [Element class][]

## Exceptions ##

* `class` **`Exception`**`: public std::`**`runtime_error`**
    * `explicit Exception::`**`Exception`**`(u8string message, size_t pos = npos)`
    * `size_t Exception::`**`pos`**`() const noexcept`
* `class` **`BadJson`**`: public Exception`
    * `explicit BadJson::`**`BadJson`**`(size_t pos)`
* `class` **`EndOfFile`**`: public Exception`
    * `explicit EndOfFile::`**`EndOfFile`**`(size_t pos)`
* `class` **`WrongType`**`: public Exception`
    * `WrongType::`**`WrongType`**`()`

TODO

## Element class ##

* `enum` **`ElementType`**
    * **`null`**
    * **`boolean`**
    * **`number`**
    * **`string`**
    * **`array`**
    * **`object`**

TODO

* `class` **`Element`**

TODO

* `using` **`Array`** `= std::vector<Element>`
* `using` **`Object`** `= std::map<u8string, Element>`

TODO

* `Element::`**`Element`**`() noexcept`
* `Element::`**`Element`**`(std::nullptr_t) noexcept`
* `Element::`**`Element`**`(bool value) noexcept`
* `template <typename T> Element::`**`Element`**`(const T& value, typename std::enable_if_t<std::is_arithmetic<T>::value>* = nullptr) noexcept`
* `Element::`**`Element`**`(const u8string& value)`
* `Element::`**`Element`**`(const char* value)`
* `Element::`**`Element`**`(const Array& value)`
* `template <typename... Args> Element::`**`Element`**`(const Args&... args)`
* `Element::`**`Element`**`(const Object& value)`
* `Element::`**`Element`**`(const Element& e)`
* `Element::`**`Element`**`(Element&& e) noexcept`
* `Element::`**`~Element`**`() noexcept`
* `Element& Element::`**`operator=`**`(const Element& e)`
* `Element& Element::`**`operator=`**`(Element&& e) noexcept`

TODO

* `Element& Element::`**`operator[]`**`(size_t key)`
* `const Element& Element::`**`operator[]`**`(size_t key) const`
* `Element& Element::`**`operator[]`**`(const u8string& key)`
* `const Element& Element::`**`operator[]`**`(const u8string& key) const`

TODO

* `bool& Element::`**`boolean`**`()`
* `bool Element::`**`boolean`**`() const`
* `double& Element::`**`number`**`()`
* `double Element::`**`number`**`() const`
* `u8string& Element::`**`string`**`()`
* `const u8string& Element::`**`string`**`() const`
* `Array& Element::`**`array`**`()`
* `const Array& Element::`**`array`**`() const`
* `Object& Element::`**`object`**`()`
* `const Object& Element::`**`object`**`() const`

TODO

* `void Element::`**`layout`**`(std::ostream& out, size_t max_array = 0) const`
* `u8string Element::`**`str`**`() const`
* `void Element::`**`write`**`(u8string& dst) const`
* `std::`**`ostream`**`& operator<<(std::ostream& out, const Element& e)`

TODO

* `ElementType Element::`**`type`**`() const noexcept`

TODO

* `static Element Element::`**`read`**`(const u8string& src)`
* `static Element Element::`**`read`**`(const u8string& src, size_t& pos)`

TODO

* `bool` **`operator==`**`(const Element& lhs, const Element& rhs) noexcept`
* `bool` **`operator!=`**`(const Element& lhs, const Element& rhs) noexcept`

TODO
