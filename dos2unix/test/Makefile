TESTS = ascii.t iso.t utf8.t misc.t

d2u_os=$(shell uname -s)
UCS = 1

# Checking WIN32 version in MSYS shell.
ifeq ($(findstring MINGW,$(d2u_os)),MINGW)
TESTS += gb18030.t
endif

ifneq ($(findstring FreeBSD,$(d2u_os)),FreeBSD)
# Unix/Linux/Cygwin. Check if zh_CN.gb18030 locale is supported.
ifeq ($(shell ./chk_loc.sh zh_CN.gb18030),yes)
TESTS += gb18030.t
endif
endif

# DOS and Windows version do not support symlinks like the Unix version.
ifeq ($(DJGPP),)
ifneq ($(findstring MINGW,$(d2u_os)),MINGW)
TESTS += symlink.t
endif
endif

ifeq ($(UCS),1)
TESTS +=utf16.t
endif

PROVE_OPT ?= --nocolor

all: test

check: test

test:
	prove -v $(PROVE_OPT) $(TESTS)


ascii:
	prove -v $(PROVE_OPT) $@.t

iso:
	prove -v $(PROVE_OPT) $@.t

misc:
	prove -v $(PROVE_OPT) $@.t

utf8:
	prove -v $(PROVE_OPT) $@.t

utf16:
	prove -v $(PROVE_OPT) $@.t

gb18030:
	prove -v $(PROVE_OPT) $@.t

symlink:
	prove -v $(PROVE_OPT) $@.t

wcstombs:
	$(CC) -Wall -Wextra wcstombs_test.c -o wcstombs_test
	@echo "====> test wcstombs() UTF-8"
	LC_ALL=en_US.UTF-8 ./wcstombs_test
	@echo "====> test wcstombs() GB18030"
	LC_ALL=zh_CN.GB18030 ./wcstombs_test

testu16: testu16.c
	gcc $< -o $@

clean:
	rm -f out*.txt in_link.txt wcstombs_test
