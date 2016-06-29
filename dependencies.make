build/$(TARGET)/character-test.o: unicorn/character-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/ucd-tables.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/character.o: unicorn/character.cpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/iso-script-names.hpp unicorn/ucd-tables.hpp
build/$(TARGET)/core-test.o: unicorn/core-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/core.o: unicorn/core.cpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/ucd-tables.hpp unicorn/property-values.hpp
build/$(TARGET)/environment-test.o: unicorn/environment-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/environment.hpp unicorn/utf.hpp \
  unicorn/character.hpp unicorn/property-values.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/environment.o: unicorn/environment.cpp unicorn/environment.hpp \
  unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp unicorn/utf.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp
build/$(TARGET)/file-test.o: unicorn/file-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/file.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/file.o: unicorn/file.cpp unicorn/file.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp unicorn/format.hpp unicorn/mbcs.hpp unicorn/regex.hpp
build/$(TARGET)/format-test.o: unicorn/format-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/format.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/format.o: unicorn/format.cpp unicorn/format.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp unicorn/regex.hpp
build/$(TARGET)/io-test.o: unicorn/io-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/io.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/file.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/io.o: unicorn/io.cpp unicorn/io.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/file.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp unicorn/format.hpp \
  unicorn/mbcs.hpp
build/$(TARGET)/lexer-test.o: unicorn/lexer-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/lexer.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/regex.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/segment.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/lexer.o: unicorn/lexer.cpp unicorn/lexer.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/regex.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/segment.hpp
build/$(TARGET)/mbcs-test.o: unicorn/mbcs-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/mbcs.hpp unicorn/utf.hpp \
  unicorn/character.hpp unicorn/property-values.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/mbcs.o: unicorn/mbcs.cpp unicorn/mbcs.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/utf.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/iana-character-sets.hpp \
  unicorn/regex.hpp unicorn/string.hpp unicorn/segment.hpp
build/$(TARGET)/normal-test.o: unicorn/normal-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/format.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp unicorn/normal.hpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/normal.o: unicorn/normal.cpp unicorn/normal.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/options-test.o: unicorn/options-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/options.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/regex.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/segment.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/options.o: unicorn/options.cpp unicorn/options.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/regex.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/format.hpp \
  unicorn/mbcs.hpp
build/$(TARGET)/regex-test.o: unicorn/regex-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/regex.hpp unicorn/utf.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/regex.o: unicorn/regex.cpp unicorn/regex.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/utf.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp
build/$(TARGET)/segment-test.o: unicorn/segment-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/segment.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp unicorn/string.hpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/segment.o: unicorn/segment.cpp unicorn/segment.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/string-algorithm-test.o: unicorn/string-algorithm-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-algorithm.o: unicorn/string-algorithm.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-case-test.o: unicorn/string-case-test.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-case.o: unicorn/string-case.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-compare-test.o: unicorn/string-compare-test.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-compare.o: unicorn/string-compare.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-conversion-test.o: unicorn/string-conversion-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-escape-test.o: unicorn/string-escape-test.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-escape.o: unicorn/string-escape.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-manip-a-e-test.o: unicorn/string-manip-a-e-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-manip-f-m-test.o: unicorn/string-manip-f-m-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-manip-n-r-test.o: unicorn/string-manip-n-r-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-manip-s-z-test.o: unicorn/string-manip-s-z-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-manip.o: unicorn/string-manip.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-property-test.o: unicorn/string-property-test.cpp \
  unicorn/string.hpp unicorn/character.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/string-property.o: unicorn/string-property.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-size-test.o: unicorn/string-size-test.cpp unicorn/string.hpp \
  unicorn/character.hpp unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp unicorn/segment.hpp unicorn/utf.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/text-table-test.o: unicorn/text-table-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/text-table.hpp unicorn/format.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/text-table.o: unicorn/text-table.cpp unicorn/text-table.hpp \
  unicorn/core.hpp $(LIBROOT)/prion-lib/prion/core.hpp unicorn/format.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/ucd-bidi-tables.o: unicorn/ucd-bidi-tables.cpp unicorn/ucd-tables.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-block-tables.o: unicorn/ucd-block-tables.cpp unicorn/ucd-tables.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-case-tables.o: unicorn/ucd-case-tables.cpp unicorn/ucd-tables.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-character-names.o: unicorn/ucd-character-names.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-decomposition-tables.o: unicorn/ucd-decomposition-tables.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-normalization-test.o: unicorn/ucd-normalization-test.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-numeric-tables.o: unicorn/ucd-numeric-tables.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-property-tables.o: unicorn/ucd-property-tables.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-script-tables.o: unicorn/ucd-script-tables.cpp unicorn/ucd-tables.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-segmentation-test.o: unicorn/ucd-segmentation-test.cpp \
  unicorn/ucd-tables.hpp $(LIBROOT)/prion-lib/prion/core.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/utf-test.o: unicorn/utf-test.cpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp \
  $(LIBROOT)/prion-lib/prion/unit-test.hpp
build/$(TARGET)/utf.o: unicorn/utf.cpp unicorn/utf.hpp unicorn/core.hpp \
  $(LIBROOT)/prion-lib/prion/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
LDLIBS += -lunicorn
LDLIBS += -lpcre
LDLIBS += -lz
