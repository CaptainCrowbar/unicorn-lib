# [Unicorn Library](index.html): String Normalization #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/normal.hpp"`

This is a small module (really just one function), with the specific purpose
of converting Unicode strings into the four standard normalization forms.

## Normalization functions ##

* `enum` **`NormalizationForm`**
    * **`NFC`**
    * **`NFD`**
    * **`NFKC`**
    * **`NFKD`**
* `std::ostream&` **`operator<<`**`(std::ostream& o, NormalizationForm n)`

The standard Unicode normalization forms.

* `Ustring` **`normalize`**`(const Ustring& src, NormalizationForm form)`
* `void` **`normalize_in`**`(Ustring& src, NormalizationForm form)`

Convert a string to one of the normalized forms. The `normalize()` function
returns the normalized string, while `normalize_in()` updates the source
string in place. As usual, these functions assume valid Unicode input, and
will emit garbage if the input contains invalid UTF-8.
