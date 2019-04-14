$(BUILD)/character-test.o: unicorn/character-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/unit-test.hpp unicorn/utility.hpp
$(BUILD)/character.o: unicorn/character.cpp unicorn/character.hpp unicorn/iso-script-names.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/environment-test.o: unicorn/environment-test.cpp unicorn/character.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/environment.o: unicorn/environment.cpp unicorn/character.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/format-test.o: unicorn/format-test.cpp unicorn/character.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/format.o: unicorn/format.cpp unicorn/character.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/io-test.o: unicorn/io-test.cpp unicorn/character.hpp unicorn/io.hpp unicorn/path.hpp unicorn/property-values.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/io.o: unicorn/io.cpp unicorn/character.hpp unicorn/format.hpp unicorn/io.hpp unicorn/mbcs.hpp unicorn/path.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/mbcs-test.o: unicorn/mbcs-test.cpp unicorn/character.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/mbcs.o: unicorn/mbcs.cpp unicorn/character.hpp unicorn/iana-character-sets.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/normal-test.o: unicorn/normal-test.cpp unicorn/character.hpp unicorn/format.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/normal.o: unicorn/normal.cpp unicorn/character.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/options-test.o: unicorn/options-test.cpp unicorn/character.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/options.o: unicorn/options.cpp unicorn/character.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/path-test.o: unicorn/path-test.cpp unicorn/character.hpp unicorn/path.hpp unicorn/property-values.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/path.o: unicorn/path.cpp unicorn/character.hpp unicorn/path.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/regex-test.o: unicorn/regex-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/regex.o: unicorn/regex.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/segment-test.o: unicorn/segment-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/segment.o: unicorn/segment.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-algorithm-test.o: unicorn/string-algorithm-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-algorithm.o: unicorn/string-algorithm.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-case-test.o: unicorn/string-case-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-case.o: unicorn/string-case.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-compare-test.o: unicorn/string-compare-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-compare.o: unicorn/string-compare.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-conversion-test.o: unicorn/string-conversion-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-escape-test.o: unicorn/string-escape-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-escape.o: unicorn/string-escape.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-manip-a-e-test.o: unicorn/string-manip-a-e-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-manip-f-m-test.o: unicorn/string-manip-f-m-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-manip-n-r-test.o: unicorn/string-manip-n-r-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-manip-s-z-test.o: unicorn/string-manip-s-z-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-manip.o: unicorn/string-manip.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-property-test.o: unicorn/string-property-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-property.o: unicorn/string-property.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/string-size-test.o: unicorn/string-size-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/ucd-bidi-tables.o: unicorn/ucd-bidi-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-block-tables.o: unicorn/ucd-block-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-case-tables.o: unicorn/ucd-case-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-character-names.o: unicorn/ucd-character-names.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-decomposition-tables.o: unicorn/ucd-decomposition-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-normalization-test.o: unicorn/ucd-normalization-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-numeric-tables.o: unicorn/ucd-numeric-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-property-tables.o: unicorn/ucd-property-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-script-tables.o: unicorn/ucd-script-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/ucd-segmentation-test.o: unicorn/ucd-segmentation-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp unicorn/utility.hpp
$(BUILD)/unit-test.o: unicorn/unit-test.cpp unicorn/unit-test.hpp unicorn/utility.hpp
$(BUILD)/utf-test.o: unicorn/utf-test.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/unit-test.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/utf.o: unicorn/utf.cpp unicorn/character.hpp unicorn/property-values.hpp unicorn/utf.hpp unicorn/utility.hpp
$(BUILD)/utility-test.o: unicorn/utility-test.cpp unicorn/unit-test.hpp unicorn/utility.hpp
ifeq ($(LIBTAG),msvc)
    LDLIBS += pcre2-8.lib zlib.lib
else
    LDLIBS += -lpcre2-8 -lz
endif
