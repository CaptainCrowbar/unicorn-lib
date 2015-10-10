Title: Unicorn Library: Environment Variables
CSS: style.css

# [Unicorn Library](index.html): Environment Variables #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/environment.hpp"` ####

## Contents ##

* [Introduction][]
* [Functions][]
* [Class Environment][]

## Introduction ##

Operating systems store environment variables in a system dependent format,
based on the string encoding supported by the native system call API. This
means that environment variable names and values are stored as 16 bit strings
on Microsoft Windows, but as 8 bit strings on Unix systems. None of the native
environment variable APIs make any attempt to enforce Unicode encoding forms;
although normally intended to be interpreted as character data, the names and
values are really arbitrary strings of 8 or 16 bit unsigned integers.

The Unicorn environment variable API allows the caller to use their preferred
UTF encoding, which means that some names and values may not be representable;
for example, if you query a variable on Windows using a UTF-8 call, the actual
value may not be valid UTF-16, and therefore not convertible to UTF-8 without
loss of information. As a compromise solution to this problem, calls that
match the native character size will, by default, simply copy strings without
attempting any kind of Unicode sanitization.

When using the functions in this module, keep in mind that, if you use the
native character type and the default behaviour flag, strings returned from
environment variable queries may not be valid Unicode. On the other hand, if
you use a different character type or a different behaviour flag, you will
always get valid Unicode strings, but they may lose some information if the
original environment data was not in UTF form.

This applies when writing environment strings too: if you set them using a
call that matches the native character size, you can pass invalid UTF and it
will simply be passed through to the underlying operating system API
unchanged; but if your call requires an encoding conversion, UTF conversion
follows the usual rules from the `[unicorn/utf](utf.html)` module.

Whether environment variable names are case sensitive is also operating system
dependent.

System environment variable APIs are usually not threadsafe; all of the
functions in this module ensure thread safety by locking a common global
mutex.

## Functions ##

The `flags` argument passed to these functions must be one of the encoding
conversion flags defined in `[unicorn/utf](utf.html)`. Behaviour is undefined
if the `flags` argument is not one of these values.

All of these functions can throw `EncodingError` if the `err_throw` flag is
used and invalid UTF encoding is encountered (either in a string supplied by
the caller, or in one returned from the environment); `std::invalid_argument`
if an environment variable name is empty or contains an equals sign, or a name
or value contains a null character; or `SystemError` (or a derived exception)
if anything else goes wrong with the underlying system calls.

* `template <typename C> basic_string<C> get_env(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> basic_string<C> get_env(const C* name, uint32_t flags = 0)`

Query the value of an environment variable. This will return an empty string
if the variable does not exist.

* `template <typename C> bool has_env(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> bool has_env(const C* name, uint32_t flags = 0)`

Query whether an environment variable exists. Windows, unlike Unix, does not
always distinguish clearly between a variable that does not exist, and one
that exists but whose value is an empty string; if a variable has been set to
an empty value, this function may or may not report its existence on Windows.

* `template <typename C> void set_env(const basic_string<C>& name, const basic_string<C>& value, uint32_t flags = 0)`
* `template <typename C> void set_env(const basic_string<C>& name, const C* value, uint32_t flags = 0)`
* `template <typename C> void set_env(const C* name, const basic_string<C>& value, uint32_t flags = 0)`
* `template <typename C> void set_env(const C* name, const C* value, uint32_t flags = 0)`

Set the value of a variable in the process's environment block.

* `template <typename C> void unset_env(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> void unset_env(const C* name, uint32_t flags = 0)`

Delete a variable from the process's environment block.

## Class Environment ##

* `class Environment`

A class that holds a complete environment block.

* `class Environment::iterator`

A read-only iterator over the name-value pairs stored in an `Environment`
object. This is a forward iterator whose value type is
`std::pair<NativeString,NativeString>`.

* `Environment::Environment()`
* `explicit Environment::Environment(bool from_process)`
* `Environment::Environment(const Environment& env)`
* `Environment::Environment(Environment&& env) noexcept`
* `Environment::~Environment() noexcept`
* `Environment& Environment::operator=(const Environment& env)`
* `Environment& Environment::operator=(Environment&& env) noexcept`

Life cycle operations. If the `from_process` flag is set, the constructor will
copy the environment data from the calling process; otherwise, the newly
constructed `Environment` object will be empty.

* `template <typename C> basic_string<C> Environment::operator[](const basic_string<C>& name)`
* `template <typename C> basic_string<C> Environment::operator[](const C* name)`
* `template <typename C> basic_string<C> Environment::get(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> basic_string<C> Environment::get(const C* name, uint32_t flags = 0)`
* `template <typename C> bool Environment::has(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> bool Environment::has(const C* name, uint32_t flags = 0)`
* `template <typename C> void Environment::set(const basic_string<C>& name, const basic_string<C>& value, uint32_t flags = 0)`
* `template <typename C> void Environment::set(const basic_string<C>& name, const C* value, uint32_t flags = 0)`
* `template <typename C> void Environment::set(const C* name, const basic_string<C>& value, uint32_t flags = 0)`
* `template <typename C> void Environment::set(const C* name, const C* value, uint32_t flags = 0)`
* `template <typename C> void Environment::unset(const basic_string<C>& name, uint32_t flags = 0)`
* `template <typename C> void Environment::unset(const C* name, uint32_t flags = 0)`

These perform the same operations as the corresponding environment variable
functions described above (`operator[]` is a synonym for `get()`). They will
throw the same exceptions (except for `SystemError`, since no system API is
being called).

* `Environment::iterator Environment::begin() const`
* `Environment::iterator Environment::end() const`

Iterators over the name-value pairs stored in an `Environment` object.

* `void Environment::clear()`

Clears all data from the environment block.

* `bool Environment::empty() const noexcept`

True if the environment block contains no data.

* `void Environment::load()`

Loads the `Environment` object's data from the calling process's environment
block. Any existing data in the `Environment` object is discarded.

* `NativeCharacter** Environment::ptr()`

Collects the environment block into a single data block in the standard format
(suitable for passing to process spawning functions, for example), and returns
a pointer to it. The returned pointer is invalidated by any operation that may
modify the `Environment` object (any call to a non-`const` function).

* `size_t Environment::size() const noexcept`

Returns the number of entries (variables) in the environment block.
