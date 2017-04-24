$(BUILD)/unicorn/character-test.o: unicorn/character-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/character.o: unicorn/character.cpp unicorn/character.hpp unicorn/core.hpp unicorn/iso-script-names.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/core-test.o: unicorn/core-test.cpp unicorn/core.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/core.o: unicorn/core.cpp unicorn/core.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/environment-test.o: unicorn/environment-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/environment.o: unicorn/environment.cpp unicorn/character.hpp unicorn/core.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/file-test.o: unicorn/file-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/file.o: unicorn/file.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/format-test.o: unicorn/format-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/format.o: unicorn/format.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/io-test.o: unicorn/io-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/io.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/io.o: unicorn/io.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/format.hpp unicorn/io.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/mbcs-test.o: unicorn/mbcs-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/mbcs.o: unicorn/mbcs.cpp unicorn/character.hpp unicorn/core.hpp unicorn/iana-character-sets.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/normal-test.o: unicorn/normal-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/normal.o: unicorn/normal.cpp unicorn/character.hpp unicorn/core.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/options-test.o: unicorn/options-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/options.o: unicorn/options.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/regex-test.o: unicorn/regex-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/regex.o: unicorn/regex.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/segment-test.o: unicorn/segment-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/segment.o: unicorn/segment.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-algorithm-test.o: unicorn/string-algorithm-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-algorithm.o: unicorn/string-algorithm.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-case-test.o: unicorn/string-case-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-case.o: unicorn/string-case.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-compare-test.o: unicorn/string-compare-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-compare.o: unicorn/string-compare.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-conversion-test.o: unicorn/string-conversion-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-escape-test.o: unicorn/string-escape-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-escape.o: unicorn/string-escape.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-manip-a-e-test.o: unicorn/string-manip-a-e-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-manip-f-m-test.o: unicorn/string-manip-f-m-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-manip-n-r-test.o: unicorn/string-manip-n-r-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-manip-s-z-test.o: unicorn/string-manip-s-z-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-manip.o: unicorn/string-manip.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-property-test.o: unicorn/string-property-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/string-property.o: unicorn/string-property.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/string-size-test.o: unicorn/string-size-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/ucd-bidi-tables.o: unicorn/ucd-bidi-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-block-tables.o: unicorn/ucd-block-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-case-tables.o: unicorn/ucd-case-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-character-names.o: unicorn/ucd-character-names.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-decomposition-tables.o: unicorn/ucd-decomposition-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-normalization-test.o: unicorn/ucd-normalization-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-numeric-tables.o: unicorn/ucd-numeric-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-property-tables.o: unicorn/ucd-property-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-script-tables.o: unicorn/ucd-script-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/ucd-segmentation-test.o: unicorn/ucd-segmentation-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/core.hpp
$(BUILD)/unicorn/utf-test.o: unicorn/utf-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/unicorn/utf.o: unicorn/utf.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/core.hpp
LDLIBS += -lpcre -lz
