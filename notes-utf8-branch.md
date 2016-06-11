## Modules in dependency order ##

* _Level 1_
    * [unchanged] `iana-character-sets`
    * [unchanged] `iso-script-names`
    * [unchanged] `property-values`
* _Level 2_
    * [unchanged] `ucd-tables -> property-values`
* _Level 3_
    * **[TODO]** `core -> ucd-tables`
    * [unchanged] `ucd-bidi-tables -> ucd-tables`
    * [unchanged] `ucd-block-tables -> ucd-tables`
    * [unchanged] `ucd-case-tables -> ucd-tables`
    * [unchanged] `ucd-character-names -> ucd-tables`
    * [unchanged] `ucd-decomposition-tables -> ucd-tables`
    * [unchanged] `ucd-numeric-tables -> ucd-tables`
    * [unchanged] `ucd-property-tables -> ucd-tables`
    * [unchanged] `ucd-script-tables -> ucd-tables`
* _Level 4_
    * **[TODO]** `character -> core, iso-script-names, property-values, ucd-tables`
* _Level 5_
    * **[TODO]** `utf -> character, core`
* _Level 6_
    * [done] `normal -> character, core, utf`
    * **[TODO]** `segment -> character, core, utf`
    * **[TODO]** `string-forward -> character, core, utf`
        * Refer to the modules these nominally belong to
* _Level 7_
    * **[TODO]** `string-size -> character, core, segment, string-forward, utf`
        * Keep all of these as templates
* _Level 8_
    * **[TODO]** `string-property -> character, core, string-forward, string-size, utf`
        * `str_char_at()`, `str_first_char()` Keep these as templates
        * Others - Make UTF-8 only
* _Level 9_
    * **[TODO]** `string-algorithm -> character, core, string-forward, string-property, string-size, utf`
        * Make all of these UTF-8 only
    * **[TODO]** `string-case -> character, core, segment, string-forward, string-property, string-size, utf`
        * Make all of these UTF-8 only
    * **[TODO]** `string-conversion -> character, core, string-forward, string-property, string-size, utf`
        * Make all of these UTF-8 only
    * **[TODO]** `string-escape -> character, core, string-forward, string-property, string-size, utf`
        * Make all of these UTF-8 only
* _Level 10_
    * **[TODO]** `string-compare -> character, core, string-algorithm, string-case, string-forward, utf`
        * `StringCompare` Delete
        * Others - Make UTF-8 only
    * **[TODO]** `string-manip -> character, core, string-algorithm, string-forward, string-property, string-size, utf`
        * `str_append()`, `str_append_char()`, `str_append_chars()` Keep these as templates
        * `str_char()`, `str_chars()` UTF-8 output only; this will require changes elsewhere
        * `str_concat()`, `str_concat_with()` UTF-8 output only, but keep the generic input
        * Others - Make UTF-8 only
* _Level 11_
    * [unchanged] `string -> string-algorithm, string-case, string-compare, string-conversion, string-escape, string-manip, string-property, string-size`
* _Level 12_
    * [done] `environment -> core, string, utf`
    * [done] `regex -> character, core, string, utf`
* _Level 13_
    * [done] `format -> character, core, regex, string, utf`
    * [done] `lexer -> character, core, regex, string`
    * [done] `mbcs -> character, core, iana-character-sets, regex, string, utf`
* _Level 14_
    * [done] `file -> character, core, format, mbcs, regex, string, utf`
    * [done] `options -> character, core, format, mbcs, regex, string`
    * [done] `table -> character, core, format, string, utf`
* _Level 15_
    * [done] `io -> character, core, file, format, mbcs, string, utf`
