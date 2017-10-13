#include "rs-core/unit-test.hpp"

extern void test_unicorn_core_character_types();
extern void test_unicorn_core_version_information();
extern void test_unicorn_character_basic_functions();
extern void test_unicorn_character_general_category();
extern void test_unicorn_character_boolean_properties();
extern void test_unicorn_character_arabic_shaping_properties();
extern void test_unicorn_character_bidirectional_properties();
extern void test_unicorn_character_block_properties();
extern void test_unicorn_character_case_folding_properties();
extern void test_unicorn_character_names();
extern void test_unicorn_character_decomposition_properties();
extern void test_unicorn_character_enumeration_properties();
extern void test_unicorn_character_numeric_properties();
extern void test_unicorn_character_script_properties();
extern void test_unicorn_character_test_all_the_things();
extern void test_unicorn_environment_query_functions();
extern void test_unicorn_environment_update_functions();
extern void test_unicorn_environment_block();
extern void test_unicorn_environment_string_expansion();
extern void test_unicorn_file_legal_names();
extern void test_unicorn_file_name_operations();
extern void test_unicorn_file_path_operations();
extern void test_unicorn_file_path_resolution();
extern void test_unicorn_file_system_operations();
extern void test_unicorn_format_booleans();
extern void test_unicorn_format_integers();
extern void test_unicorn_format_floating_point();
extern void test_unicorn_format_characters();
extern void test_unicorn_format_strings();
extern void test_unicorn_format_date_and_time();
extern void test_unicorn_format_uuid();
extern void test_unicorn_format_version();
extern void test_unicorn_format_ranges();
extern void test_unicorn_format_alignment_and_padding();
extern void test_unicorn_format_case_mapping();
extern void test_unicorn_format_class();
extern void test_unicorn_format_literals();
extern void test_unicorn_io_file_reader();
extern void test_unicorn_io_file_writer();
extern void test_unicorn_mbcs_locale_detection();
extern void test_unicorn_mbcs_utf_detection();
extern void test_unicorn_mbcs_encoding_queries();
extern void test_unicorn_mbcs_to_unicode();
extern void test_unicorn_mbcs_from_unicode();
extern void test_unicorn_mbcs_local_encoding_round_trip();
extern void test_unicorn_normal_normalization();
extern void test_unicorn_options_basic();
extern void test_unicorn_options_boolean();
extern void test_unicorn_options_multiple();
extern void test_unicorn_options_required();
extern void test_unicorn_options_anonymous();
extern void test_unicorn_options_group();
extern void test_unicorn_options_patterns();
extern void test_unicorn_options_help();
extern void test_unicorn_options_insertions();
extern void test_unicorn_regex_version_information();
extern void test_unicorn_regex_utf8();
extern void test_unicorn_regex_match_ranges();
extern void test_unicorn_regex_split_ranges();
extern void test_unicorn_regex_formatting();
extern void test_unicorn_regex_transform();
extern void test_unicorn_regex_string_escaping();
extern void test_unicorn_regex_bytes();
extern void test_unicorn_regex_literals();
extern void test_unicorn_segment_graphemes();
extern void test_unicorn_segment_words();
extern void test_unicorn_segment_lines();
extern void test_unicorn_segment_sentences();
extern void test_unicorn_segment_paragraphs();
extern void test_unicorn_string_algorithm_common();
extern void test_unicorn_string_algorithm_expect();
extern void test_unicorn_string_algorithm_find_char();
extern void test_unicorn_string_algorithm_find_first();
extern void test_unicorn_string_algorithm_line_column();
extern void test_unicorn_string_algorithm_search();
extern void test_unicorn_string_algorithm_skipws();
extern void test_unicorn_string_case_conversions();
extern void test_unicorn_string_compare_3_way();
extern void test_unicorn_string_compare_case_insensitive();
extern void test_unicorn_string_compare_natural();
extern void test_unicorn_string_compare_utf_compare();
extern void test_unicorn_string_conversion_decimal_integers();
extern void test_unicorn_string_conversion_hexadecimal_integers();
extern void test_unicorn_string_conversion_floating_point();
extern void test_unicorn_string_escape_encode_uri();
extern void test_unicorn_string_escape_backslash();
extern void test_unicorn_string_escape_quote();
extern void test_unicorn_string_manip_append();
extern void test_unicorn_string_manip_chars();
extern void test_unicorn_string_manip_concat();
extern void test_unicorn_string_manip_drop();
extern void test_unicorn_string_manip_erase();
extern void test_unicorn_string_manip_expand();
extern void test_unicorn_string_manip_fix_left();
extern void test_unicorn_string_manip_fix_right();
extern void test_unicorn_string_manip_insert();
extern void test_unicorn_string_manip_join();
extern void test_unicorn_string_manip_pad_left();
extern void test_unicorn_string_manip_pad_right();
extern void test_unicorn_string_manip_partition();
extern void test_unicorn_string_manip_remove();
extern void test_unicorn_string_manip_repeat();
extern void test_unicorn_string_manip_replace();
extern void test_unicorn_string_manip_split();
extern void test_unicorn_string_manip_squeeze();
extern void test_unicorn_string_manip_substring();
extern void test_unicorn_string_manip_translate();
extern void test_unicorn_string_manip_trim();
extern void test_unicorn_string_manip_trim_if();
extern void test_unicorn_string_manip_unify();
extern void test_unicorn_string_manip_wrap();
extern void test_unicorn_string_property_char_at();
extern void test_unicorn_string_property_first_and_last();
extern void test_unicorn_string_property_east_asian();
extern void test_unicorn_string_property_starts_and_ends();
extern void test_unicorn_string_size_measurement_flags();
extern void test_unicorn_string_size_find_offset();
extern void test_unicorn_utf_basic_conversions();
extern void test_unicorn_utf_basic_utilities();
extern void test_unicorn_utf_decoding_iterators();
extern void test_unicorn_utf_decoding_ranges();
extern void test_unicorn_utf_implicit_recoding();
extern void test_unicorn_utf_explicit_recoding();
extern void test_unicorn_utf_string_validation();
extern void test_unicorn_utf_error_handling();

int main() {

    static const RS::UnitTest::test_index index = {

        { "unicorn/core/character-types", test_unicorn_core_character_types },
        { "unicorn/core/version-information", test_unicorn_core_version_information },
        { "unicorn/character/basic-functions", test_unicorn_character_basic_functions },
        { "unicorn/character/general-category", test_unicorn_character_general_category },
        { "unicorn/character/boolean-properties", test_unicorn_character_boolean_properties },
        { "unicorn/character/arabic-shaping-properties", test_unicorn_character_arabic_shaping_properties },
        { "unicorn/character/bidirectional-properties", test_unicorn_character_bidirectional_properties },
        { "unicorn/character/block-properties", test_unicorn_character_block_properties },
        { "unicorn/character/case-folding-properties", test_unicorn_character_case_folding_properties },
        { "unicorn/character/names", test_unicorn_character_names },
        { "unicorn/character/decomposition-properties", test_unicorn_character_decomposition_properties },
        { "unicorn/character/enumeration-properties", test_unicorn_character_enumeration_properties },
        { "unicorn/character/numeric-properties", test_unicorn_character_numeric_properties },
        { "unicorn/character/script-properties", test_unicorn_character_script_properties },
        { "unicorn/character/test-all-the-things", test_unicorn_character_test_all_the_things },
        { "unicorn/environment/query-functions", test_unicorn_environment_query_functions },
        { "unicorn/environment/update-functions", test_unicorn_environment_update_functions },
        { "unicorn/environment/block", test_unicorn_environment_block },
        { "unicorn/environment/string-expansion", test_unicorn_environment_string_expansion },
        { "unicorn/file/legal-names", test_unicorn_file_legal_names },
        { "unicorn/file/name-operations", test_unicorn_file_name_operations },
        { "unicorn/file/path-operations", test_unicorn_file_path_operations },
        { "unicorn/file/path-resolution", test_unicorn_file_path_resolution },
        { "unicorn/file/system-operations", test_unicorn_file_system_operations },
        { "unicorn/format/booleans", test_unicorn_format_booleans },
        { "unicorn/format/integers", test_unicorn_format_integers },
        { "unicorn/format/floating-point", test_unicorn_format_floating_point },
        { "unicorn/format/characters", test_unicorn_format_characters },
        { "unicorn/format/strings", test_unicorn_format_strings },
        { "unicorn/format/date-and-time", test_unicorn_format_date_and_time },
        { "unicorn/format/uuid", test_unicorn_format_uuid },
        { "unicorn/format/version", test_unicorn_format_version },
        { "unicorn/format/ranges", test_unicorn_format_ranges },
        { "unicorn/format/alignment-and-padding", test_unicorn_format_alignment_and_padding },
        { "unicorn/format/case-mapping", test_unicorn_format_case_mapping },
        { "unicorn/format/class", test_unicorn_format_class },
        { "unicorn/format/literals", test_unicorn_format_literals },
        { "unicorn/io/file-reader", test_unicorn_io_file_reader },
        { "unicorn/io/file-writer", test_unicorn_io_file_writer },
        { "unicorn/mbcs/locale-detection", test_unicorn_mbcs_locale_detection },
        { "unicorn/mbcs/utf-detection", test_unicorn_mbcs_utf_detection },
        { "unicorn/mbcs/encoding-queries", test_unicorn_mbcs_encoding_queries },
        { "unicorn/mbcs/to-unicode", test_unicorn_mbcs_to_unicode },
        { "unicorn/mbcs/from-unicode", test_unicorn_mbcs_from_unicode },
        { "unicorn/mbcs/local-encoding-round-trip", test_unicorn_mbcs_local_encoding_round_trip },
        { "unicorn/normal/normalization", test_unicorn_normal_normalization },
        { "unicorn/options/basic", test_unicorn_options_basic },
        { "unicorn/options/boolean", test_unicorn_options_boolean },
        { "unicorn/options/multiple", test_unicorn_options_multiple },
        { "unicorn/options/required", test_unicorn_options_required },
        { "unicorn/options/anonymous", test_unicorn_options_anonymous },
        { "unicorn/options/group", test_unicorn_options_group },
        { "unicorn/options/patterns", test_unicorn_options_patterns },
        { "unicorn/options/help", test_unicorn_options_help },
        { "unicorn/options/insertions", test_unicorn_options_insertions },
        { "unicorn/regex/version-information", test_unicorn_regex_version_information },
        { "unicorn/regex/utf8", test_unicorn_regex_utf8 },
        { "unicorn/regex/match-ranges", test_unicorn_regex_match_ranges },
        { "unicorn/regex/split-ranges", test_unicorn_regex_split_ranges },
        { "unicorn/regex/formatting", test_unicorn_regex_formatting },
        { "unicorn/regex/transform", test_unicorn_regex_transform },
        { "unicorn/regex/string-escaping", test_unicorn_regex_string_escaping },
        { "unicorn/regex/bytes", test_unicorn_regex_bytes },
        { "unicorn/regex/literals", test_unicorn_regex_literals },
        { "unicorn/segment/graphemes", test_unicorn_segment_graphemes },
        { "unicorn/segment/words", test_unicorn_segment_words },
        { "unicorn/segment/lines", test_unicorn_segment_lines },
        { "unicorn/segment/sentences", test_unicorn_segment_sentences },
        { "unicorn/segment/paragraphs", test_unicorn_segment_paragraphs },
        { "unicorn/string-algorithm/common", test_unicorn_string_algorithm_common },
        { "unicorn/string-algorithm/expect", test_unicorn_string_algorithm_expect },
        { "unicorn/string-algorithm/find-char", test_unicorn_string_algorithm_find_char },
        { "unicorn/string-algorithm/find-first", test_unicorn_string_algorithm_find_first },
        { "unicorn/string-algorithm/line-column", test_unicorn_string_algorithm_line_column },
        { "unicorn/string-algorithm/search", test_unicorn_string_algorithm_search },
        { "unicorn/string-algorithm/skipws", test_unicorn_string_algorithm_skipws },
        { "unicorn/string-case/conversions", test_unicorn_string_case_conversions },
        { "unicorn/string-compare/3-way", test_unicorn_string_compare_3_way },
        { "unicorn/string-compare/case-insensitive", test_unicorn_string_compare_case_insensitive },
        { "unicorn/string-compare/natural", test_unicorn_string_compare_natural },
        { "unicorn/string-compare/utf-compare", test_unicorn_string_compare_utf_compare },
        { "unicorn/string-conversion/decimal-integers", test_unicorn_string_conversion_decimal_integers },
        { "unicorn/string-conversion/hexadecimal-integers", test_unicorn_string_conversion_hexadecimal_integers },
        { "unicorn/string-conversion/floating-point", test_unicorn_string_conversion_floating_point },
        { "unicorn/string-escape/encode-uri", test_unicorn_string_escape_encode_uri },
        { "unicorn/string-escape/backslash", test_unicorn_string_escape_backslash },
        { "unicorn/string-escape/quote", test_unicorn_string_escape_quote },
        { "unicorn/string-manip/append", test_unicorn_string_manip_append },
        { "unicorn/string-manip/chars", test_unicorn_string_manip_chars },
        { "unicorn/string-manip/concat", test_unicorn_string_manip_concat },
        { "unicorn/string-manip/drop", test_unicorn_string_manip_drop },
        { "unicorn/string-manip/erase", test_unicorn_string_manip_erase },
        { "unicorn/string-manip/expand", test_unicorn_string_manip_expand },
        { "unicorn/string-manip/fix-left", test_unicorn_string_manip_fix_left },
        { "unicorn/string-manip/fix-right", test_unicorn_string_manip_fix_right },
        { "unicorn/string-manip/insert", test_unicorn_string_manip_insert },
        { "unicorn/string-manip/join", test_unicorn_string_manip_join },
        { "unicorn/string-manip/pad-left", test_unicorn_string_manip_pad_left },
        { "unicorn/string-manip/pad-right", test_unicorn_string_manip_pad_right },
        { "unicorn/string-manip/partition", test_unicorn_string_manip_partition },
        { "unicorn/string-manip/remove", test_unicorn_string_manip_remove },
        { "unicorn/string-manip/repeat", test_unicorn_string_manip_repeat },
        { "unicorn/string-manip/replace", test_unicorn_string_manip_replace },
        { "unicorn/string-manip/split", test_unicorn_string_manip_split },
        { "unicorn/string-manip/squeeze", test_unicorn_string_manip_squeeze },
        { "unicorn/string-manip/substring", test_unicorn_string_manip_substring },
        { "unicorn/string-manip/translate", test_unicorn_string_manip_translate },
        { "unicorn/string-manip/trim", test_unicorn_string_manip_trim },
        { "unicorn/string-manip/trim-if", test_unicorn_string_manip_trim_if },
        { "unicorn/string-manip/unify", test_unicorn_string_manip_unify },
        { "unicorn/string-manip/wrap", test_unicorn_string_manip_wrap },
        { "unicorn/string-property/char-at", test_unicorn_string_property_char_at },
        { "unicorn/string-property/first-and-last", test_unicorn_string_property_first_and_last },
        { "unicorn/string-property/east-asian", test_unicorn_string_property_east_asian },
        { "unicorn/string-property/starts-and-ends", test_unicorn_string_property_starts_and_ends },
        { "unicorn/string-size/measurement-flags", test_unicorn_string_size_measurement_flags },
        { "unicorn/string-size/find-offset", test_unicorn_string_size_find_offset },
        { "unicorn/utf/basic-conversions", test_unicorn_utf_basic_conversions },
        { "unicorn/utf/basic-utilities", test_unicorn_utf_basic_utilities },
        { "unicorn/utf/decoding-iterators", test_unicorn_utf_decoding_iterators },
        { "unicorn/utf/decoding-ranges", test_unicorn_utf_decoding_ranges },
        { "unicorn/utf/implicit-recoding", test_unicorn_utf_implicit_recoding },
        { "unicorn/utf/explicit-recoding", test_unicorn_utf_explicit_recoding },
        { "unicorn/utf/string-validation", test_unicorn_utf_string_validation },
        { "unicorn/utf/error-handling", test_unicorn_utf_error_handling },

    };

    return RS::UnitTest::test_main(index);

}
