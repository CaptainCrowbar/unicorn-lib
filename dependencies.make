build/$(TARGET)/character-test.o: unicorn/character-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/ucd-tables.hpp
build/$(TARGET)/character.o: unicorn/character.cpp unicorn/character.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/property-values.hpp \
  unicorn/iso-script-names.hpp unicorn/ucd-tables.hpp
build/$(TARGET)/core-test.o: unicorn/core-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp
build/$(TARGET)/core.o: unicorn/core.cpp unicorn/character.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/property-values.hpp \
  unicorn/ucd-tables.hpp
build/$(TARGET)/file-test.o: unicorn/file-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/file.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/file.o: unicorn/file.cpp unicorn/file.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp unicorn/format.hpp unicorn/regex.hpp unicorn/mbcs.hpp
build/$(TARGET)/format-test.o: unicorn/format-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/format.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/regex.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/format.o: unicorn/format.cpp unicorn/format.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/regex.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/io-test.o: unicorn/io-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/io.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/file.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/io.o: unicorn/io.cpp unicorn/io.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/file.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp unicorn/format.hpp \
  unicorn/regex.hpp unicorn/mbcs.hpp
build/$(TARGET)/lexer-test.o: unicorn/lexer-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/lexer.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/format.hpp \
  unicorn/regex.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp
build/$(TARGET)/mbcs-test.o: unicorn/mbcs-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/mbcs.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/mbcs.o: unicorn/mbcs.cpp unicorn/mbcs.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp \
  unicorn/iana-character-sets.hpp unicorn/regex.hpp unicorn/string.hpp \
  unicorn/segment.hpp
build/$(TARGET)/normal-test.o: unicorn/normal-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/normal.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/ucd-tables.hpp
build/$(TARGET)/normal.o: unicorn/normal.cpp unicorn/normal.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/options-test.o: unicorn/options-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/options.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/format.hpp \
  unicorn/regex.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp
build/$(TARGET)/options.o: unicorn/options.cpp unicorn/options.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/format.hpp unicorn/regex.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp \
  unicorn/mbcs.hpp
build/$(TARGET)/regex-test.o: unicorn/regex-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/regex.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/string.hpp \
  unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/regex.o: unicorn/regex.cpp unicorn/regex.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/string.hpp unicorn/segment.hpp \
  unicorn/utf.hpp
build/$(TARGET)/segment-test.o: unicorn/segment-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/segment.hpp \
  unicorn/character.hpp unicorn/property-values.hpp unicorn/utf.hpp \
  unicorn/string.hpp unicorn/ucd-tables.hpp
build/$(TARGET)/segment.o: unicorn/segment.cpp unicorn/segment.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/string-algorithm-test.o: unicorn/string-algorithm-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-case-test.o: unicorn/string-case-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-conversion-test.o: unicorn/string-conversion-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-manip-a-e-test.o: unicorn/string-manip-a-e-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-manip-f-m-test.o: unicorn/string-manip-f-m-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-manip-n-r-test.o: unicorn/string-manip-n-r-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-manip-s-z-test.o: unicorn/string-manip-s-z-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-property-test.o: unicorn/string-property-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/string-size-test.o: unicorn/string-size-test.cpp \
  ../crow-lib/crow/unit-test.hpp ../crow-lib/crow/core.hpp \
  unicorn/core.hpp unicorn/character.hpp unicorn/property-values.hpp \
  unicorn/string.hpp unicorn/segment.hpp unicorn/utf.hpp
build/$(TARGET)/ucd-bidi-tables.o: unicorn/ucd-bidi-tables.cpp unicorn/ucd-tables.hpp \
  unicorn/core.hpp ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-block-tables.o: unicorn/ucd-block-tables.cpp unicorn/ucd-tables.hpp \
  unicorn/core.hpp ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-case-tables.o: unicorn/ucd-case-tables.cpp unicorn/ucd-tables.hpp \
  unicorn/core.hpp ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-character-names.o: unicorn/ucd-character-names.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-decomposition-tables.o: unicorn/ucd-decomposition-tables.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-normalization-test.o: unicorn/ucd-normalization-test.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-numeric-tables.o: unicorn/ucd-numeric-tables.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-property-tables.o: unicorn/ucd-property-tables.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/ucd-script-tables.o: unicorn/ucd-script-tables.cpp unicorn/ucd-tables.hpp \
  unicorn/core.hpp ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
build/$(TARGET)/ucd-segmentation-test.o: unicorn/ucd-segmentation-test.cpp \
  unicorn/ucd-tables.hpp unicorn/core.hpp ../crow-lib/crow/core.hpp \
  unicorn/character.hpp unicorn/property-values.hpp
build/$(TARGET)/utf-test.o: unicorn/utf-test.cpp ../crow-lib/crow/unit-test.hpp \
  ../crow-lib/crow/core.hpp unicorn/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp unicorn/utf.hpp
build/$(TARGET)/utf.o: unicorn/utf.cpp unicorn/utf.hpp unicorn/core.hpp \
  ../crow-lib/crow/core.hpp unicorn/character.hpp \
  unicorn/property-values.hpp
