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
    * [unchanged] `segment -> character, core, utf`
    * [done] `string-forward -> character, core, utf`
* _Level 7_
    * [unchanged] `string-size -> character, core, segment, string-forward, utf`
* _Level 8_
    * [done] `string-property -> character, core, string-forward, string-size, utf`
* _Level 9_
    * [done] `string-algorithm -> character, core, string-forward, string-property, string-size, utf`
    * [done] `string-case -> character, core, segment, string-forward, string-property, string-size, utf`
    * [done] `string-conversion -> character, core, string-forward, string-property, string-size, utf`
    * [done] `string-escape -> character, core, string-forward, string-property, string-size, utf`
* _Level 10_
    * [done] `string-compare -> character, core, string-algorithm, string-case, string-forward, utf`
    * [done] `string-manip -> character, core, string-algorithm, string-forward, string-property, string-size, utf`
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
