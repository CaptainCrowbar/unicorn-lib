Title: Unicorn Library: String Normalization
CSS: style.css

# [Unicorn Library](index.html): String Normalization #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/normal.hpp"` ####

This is a small module (really just one function), with the specific purpose
of converting Unicode strings into the four standard normalization forms.

## Normalization functions ##

* `enum NormalizationForm`
    * `NFC, NFD, NFKC, NFKD`

The standard Unicode normalization forms.

* `template <typename C> basic_string<C> normalize(const basic_string<C>& src, NormalizationForm form)`
* `template <typename C> void normalize_in(basic_string<C>& src, NormalizationForm form)`

Convert a string to one of the normalized forms. The `normalize()` function
returns the normalized string, while `normalize_in()` updates the source
string in place. As usual, these functions assume valid Unicode input, and
will emit garbage if the input contains invalid UTF encoding.
