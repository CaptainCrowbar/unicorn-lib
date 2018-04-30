# Unicorn Makefile

# Project identity

project_name := unicorn
dependency_file := dependencies.make
install_prefix := /usr/local

# Toolset identity

build_host := $(shell uname | tr A-Z a-z | sed -E 's/[^a-z].*//')

ifeq ($(TOOLS),msvc)
	build_target := $(shell cl 2>&1 | head -n 1 | sed -E 's/^.*Version ([0-9]+)\.([0-9]+)\.[0-9.]+ for (.+)$$/msvc-\1\2-\3/')
	cross_target := msvc
else
	build_target := $(shell gcc -v 2>&1 | grep '^Target:' | sed -E -e 's/^Target: //' -e 's/[.].*$$//' | tr A-Z a-z)
	cross_target := $(shell echo $(build_target) | tr A-Z a-z | sed -E -e 's/-gnu$$//' -e 's/.*-//' -e 's/[^a-z].*//')
endif

# Toolset configuration

# Start with generic Unix settings, then modify based on the toolset.
# Empty comments are a makefile trick to create a variable value with a trailing space.

BUILD := build/$(build_target)
CXX := g++
common_flags := -march=ivybridge -mfpmath=sse
diagnostic_flags := -fdiagnostics-color=always -Wall -Wextra -Werror
cxx_specific_flags :=
cxx_defines := -DNDEBUG=1
nontest_flags := -O2
test_flags := -O1
cxx_output := -o #
include_path := -I.
AR := ar
ar_specific_flags := -rsu
ar_output :=
LDLIBS :=
ld_specific_flags :=
ld_output := -o #
library_path := -L$(BUILD)
lib_prefix := lib
lib_suffix := .a
exe_suffix :=

ifeq ($(build_host),cygwin)
	exe_suffix := .exe
endif

ifeq ($(cross_target),msvc)
	install_prefix := ~/Dropbox/Lib/msvc
	windows_prefix := $(shell cygpath -aw $(install_prefix))
	vcpkg_prefix := $(VCPKG_ROOT)/installed/x64-windows
	CXX := cl
	common_flags := /EHc /EHs /fp:precise /Gy /MD /MP /nologo /sdl /utf-8 /Y-
	diagnostic_flags := /W4 /WX
	cxx_specific_flags := /permissive- /std:c++latest
	cxx_defines := /D_CRT_SECURE_NO_WARNINGS=1 /DNDEBUG=1 /DNOMINMAX=1 /DUNICODE=1 /D_UNICODE=1 /DWINVER=0x601 /D_WIN32_WINNT=0x601
	nontest_flags := /O2
	test_flags :=
	cxx_output := /Fo
	include_path := /I. /I$(windows_prefix)/include /I$(vcpkg_prefix)/include
	AR := lib
	ar_specific_flags := /NOLOGO
	ar_output := /OUT:
	LD := link
	LDLIBS :=
	ld_specific_flags := /CGTHREADS:4 /MACHINE:X64 /NOLOGO /NXCOMPAT /SUBSYSTEM:CONSOLE
	ld_output := /OUT:
	library_path := /LIBPATH:$(BUILD) /LIBPATH:$(windows_prefix)/lib /LIBPATH:$(vcpkg_prefix)/lib
	lib_prefix :=
	lib_suffix := .lib
endif

ifeq ($(cross_target),cygwin)
	cxx_defines += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=gnu++1z
	ld_specific_flags += -Wl,--enable-auto-import
endif

ifeq ($(cross_target),darwin)
	CXX := clang++
	cxx_defines += -D_DARWIN_C_SOURCE=1 -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=c++1z -stdlib=libc++
	ld_specific_flags += -framework Cocoa
endif

ifeq ($(cross_target),linux)
	cxx_defines += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=gnu++1z
endif

# Collect the build tools and their options

CXXFLAGS := $(common_flags) $(diagnostic_flags) $(include_path) $(cxx_specific_flags) $(cxx_defines) $(nontest_flags)
test_cxxflags := $(common_flags) $(diagnostic_flags) $(include_path) $(cxx_specific_flags) $(cxx_defines) $(test_flags)

ifeq ($(cross_target),msvc)
	ARFLAGS := $(ar_specific_flags) $(library_path)
	LDFLAGS := $(ld_specific_flags) $(library_path)
	test_ldflags := $(ld_specific_flags) $(library_path)
else
	ARFLAGS := $(ar_specific_flags)
	LD := $(CXX)
	LDFLAGS := $(common_flags) $(diagnostic_flags) $(cxx_defines) $(nontest_flags) $(ld_specific_flags) $(library_path)
	test_ldflags := $(common_flags) $(diagnostic_flags) $(cxx_defines) $(test_flags) $(ld_specific_flags) $(library_path)
endif

# File names

all_headers := $(shell find $(project_name) -name *.hpp)
library_headers := $(filter-out $(project_name)/library.hpp Makefile,$(shell grep -EL '// NOT INSTALLED' $(all_headers) Makefile)) # Dummy entry to avoid empty list
all_sources := $(shell find $(project_name) -name *.c -or -name *.cpp -or -name *.m -or -name *.mm)
test_sources := $(shell echo "$(all_sources)" | tr ' ' '\n' | grep '[-]test')
test_sources := $(sort $(test_sources) $(project_name)/unit-test.cpp)
library_sources := $(filter-out $(app_sources) $(test_sources),$(all_sources))
test_objects := $(shell sed -E 's!$(project_name)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(test_sources)')
library_objects := $(shell sed -E 's!$(project_name)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(library_sources)')
doc_index := $(wildcard $(project_name)/index.md)
doc_sources := $(shell find $(project_name) -name '*.md' | sort)
doc_pages := doc/style.css doc/index.html $(patsubst $(project_name)/%.md,doc/%.html,$(doc_sources))
static_name := $(lib_prefix)$(project_name)$(lib_suffix)
static_library := $(BUILD)/$(static_name)
test_name := test-$(project_name)$(exe_suffix)
test_target := $(BUILD)/$(test_name)
static_target := $(static_library)
install_include := $(install_prefix)/include/$(project_name)
install_static := $(install_prefix)/lib/$(static_name)

# Common build targets

.DELETE_ON_ERROR:

.PHONY: all static tests check install symlinks uninstall doc unlink undoc clean clean-all dep help print-%

all: static tests doc

unlink:
	rm -f $(static_library) $(test_target)

undoc:
	rm -f doc/*.html

clean:
	rm -rf $(BUILD) *.stackdump __test_*

clean-all:
	rm -rf build doc *.stackdump __test_*

dep:
	scripts/make-dependencies

help:
	@echo
	@echo "Make targets:"
	@echo
	@echo "    all        = Build everything (default)"
	@echo "    static     = Build the static library"
	@echo "    tests      = Build the unit tests"
	@echo "    check      = Build everything and run the unit tests"
	@echo "    install    = Install the application"
	@echo "    symlinks   = Install the application using symlinks"
	@echo "    uninstall  = Uninstall the application"
	@echo "    doc        = Build the documentation"
	@echo "    unlink     = Delete link targets but not object modules"
	@echo "    undoc      = Delete the documentation"
	@echo "    clean      = Delete all targets for this toolset"
	@echo "    clean-all  = Delete all targets for all toolsets and docs"
	@echo "    dep        = Rebuild makefile dependencies"
	@echo "    help       = List make targets"
	@echo "    print-*    = Query a makefile variable"

print-%:
	@echo "$* = $($*)"

# Unit test targets

tests: $(test_target)

check: all
	@rm -rf __test_*
	$(test_target)
	@rm -rf __test_*

# Documentation targets

doc: $(doc_pages)

# Library targets

static: $(static_target) $(project_name)/library.hpp

install: uninstall static
	@mkdir -p $(install_include) $(dir $(install_static))
	cp $(library_headers) $(project_name)/library.hpp $(install_include)
	cp $(static_library) $(install_static)

uninstall:
	if [ -h "$(install_include)" ]; then rm $(install_include); fi
	if [ -d "$(install_include)" ]; then rm -rf $(install_include); fi
	if [ -f "$(install_static)" ] || [ -h "$(install_static)" ]; then rm $(install_static); fi

ifeq ($(cross_target),msvc)

symlinks:
	$(error make symlinks is not supported on $(cross_target), use make install)

else

symlinks: uninstall static
	@mkdir -p $(dir $(install_include)) $(dir $(install_static))
	ln -s $(abspath $(project_name)) $(install_include)
	ln -s $(abspath $(static_library)) $(install_static)

endif

# Include the dependency list

-include $(dependency_file)

# System specific link libraries

ifeq ($(cross_target),msvc)
    LDLIBS += pcre.lib zlib.lib shell32.lib advapi32.lib
else
	LDLIBS += -lpcre -lpthread -lz
	ifneq ($(cross_target),linux)
		LDLIBS += -liconv
	endif
endif

# Rules for building objects from source

$(BUILD)/%-test.o: $(project_name)/%-test.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(test_cxxflags) -c $< $(cxx_output)$@

$(BUILD)/%.o: $(project_name)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< $(cxx_output)$@

# Rules for building the final target from objects

$(static_library): $(library_objects)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(AR) $(ARFLAGS) $(ar_output)$@ $^

$(test_target): $(test_objects) $(static_target)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(test_ldflags) $^ $(LDLIBS) $(ld_output)$@

# Other build rules

$(project_name)/library.hpp: $(library_headers)
	echo "#pragma once" > $@
	echo >> $@
	echo $(sort $(library_headers)) | tr ' ' '\n' | sed -E 's/.+/#include "&"/' >> $@

$(project_name)/unit-test.cpp: $(filter-out $(project_name)/unit-test.cpp,$(test_sources))
	scripts/make-tests

doc/%.html: $(project_name)/%.md scripts/make-doc
	@mkdir -p $(dir $@)
	scripts/make-doc $< $@

doc/style.css: scripts/style.css
	@mkdir -p $(dir $@)
	cp $< $@
