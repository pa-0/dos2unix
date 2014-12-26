TESTS = ascii.t iso.t utf8.t utf16.t misc.t

d2u_os=$(shell uname -s)

# GB18030 test is not working on Cygwin and MSYS. They do not support zh_CN.GB18030 locale.
ifeq ($(findstring CYGWIN,$(d2u_os)),)
ifeq ($(findstring MSYS,$(d2u_os)),)
TESTS += gb18030.t
endif
endif

# Windows version does not support symlinks like the Unix version.
ifneq ($(findstring MINGW,$(d2u_os)),MINGW)
TESTS += symlink.t
endif


all: test

check: test

test:
	prove -v $(TESTS)


ascii:
	prove -v $@.t

iso:
	prove -v $@.t

misc:
	prove -v $@.t

utf8:
	prove -v $@.t

utf16:
	prove -v $@.t

gb18030:
	prove -v $@.t

symlink:
	prove -v $@.t

clean:
	rm -f out*.txt in_link.txt
