$(BUILD)/character-test.o: unicorn/character-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/character.o: unicorn/character.cpp unicorn/character.hpp unicorn/core.hpp unicorn/iso-script-names.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/core-test.o: unicorn/core-test.cpp unicorn/core.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/core.o: unicorn/core.cpp unicorn/core.hpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/environment-test.o: unicorn/environment-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/environment.o: unicorn/environment.cpp unicorn/character.hpp unicorn/core.hpp unicorn/environment.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/file-test.o: unicorn/file-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/file.o: unicorn/file.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/format-test.o: unicorn/format-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/unit-test.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/format.o: unicorn/format.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/io-test.o: unicorn/io-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/io.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/file.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/io.o: unicorn/io.cpp unicorn/character.hpp unicorn/core.hpp unicorn/file.hpp unicorn/format.hpp unicorn/io.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/mbcs-test.o: unicorn/mbcs-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/mbcs.o: unicorn/mbcs.cpp unicorn/character.hpp unicorn/core.hpp unicorn/iana-character-sets.hpp unicorn/mbcs.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/normal-test.o: unicorn/normal-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/unit-test.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/normal.o: unicorn/normal.cpp unicorn/character.hpp unicorn/core.hpp unicorn/normal.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/options-test.o: unicorn/options-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/options.o: unicorn/options.cpp unicorn/character.hpp unicorn/core.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/options.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/float.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/random.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/time.hpp /usr/local/include/rs-core/uuid.hpp /usr/local/include/rs-core/vector.hpp
$(BUILD)/regex-test.o: unicorn/regex-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/regex.o: unicorn/regex.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/regex.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/segment-test.o: unicorn/segment-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/ucd-tables.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/segment.o: unicorn/segment.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-algorithm-test.o: unicorn/string-algorithm-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-algorithm.o: unicorn/string-algorithm.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-case-test.o: unicorn/string-case-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-case.o: unicorn/string-case.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-compare-test.o: unicorn/string-compare-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-compare.o: unicorn/string-compare.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-conversion-test.o: unicorn/string-conversion-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-escape-test.o: unicorn/string-escape-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-escape.o: unicorn/string-escape.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-manip-a-e-test.o: unicorn/string-manip-a-e-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-manip-f-m-test.o: unicorn/string-manip-f-m-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-manip-n-r-test.o: unicorn/string-manip-n-r-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-manip-s-z-test.o: unicorn/string-manip-s-z-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-manip.o: unicorn/string-manip.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-property-test.o: unicorn/string-property-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/string-property.o: unicorn/string-property.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
$(BUILD)/string-size-test.o: unicorn/string-size-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/segment.hpp unicorn/string.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/kwargs.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/ucd-bidi-tables.o: unicorn/ucd-bidi-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-block-tables.o: unicorn/ucd-block-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-case-tables.o: unicorn/ucd-case-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-character-names.o: unicorn/ucd-character-names.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-decomposition-tables.o: unicorn/ucd-decomposition-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-normalization-test.o: unicorn/ucd-normalization-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-numeric-tables.o: unicorn/ucd-numeric-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-property-tables.o: unicorn/ucd-property-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-script-tables.o: unicorn/ucd-script-tables.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/ucd-segmentation-test.o: unicorn/ucd-segmentation-test.cpp unicorn/property-values.hpp unicorn/ucd-tables.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp
$(BUILD)/unit-test.o: unicorn/unit-test.cpp  \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/utf-test.o: unicorn/utf-test.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp /usr/local/include/rs-core/unit-test.hpp
$(BUILD)/utf.o: unicorn/utf.cpp unicorn/character.hpp unicorn/core.hpp unicorn/property-values.hpp unicorn/utf.hpp \
    /usr/local/include/rs-core/common.hpp /usr/local/include/rs-core/meta-internal.hpp /usr/local/include/rs-core/meta.hpp /usr/local/include/rs-core/string.hpp
ifeq ($(LIBTAG),msvc)
    LDLIBS += pcre.lib zlib.lib
else
    LDLIBS += -lpcre -lz
endif
ifeq ($(LIBTAG),msvc)
    LDLIBS += shell32.lib advapi32.lib
endif
