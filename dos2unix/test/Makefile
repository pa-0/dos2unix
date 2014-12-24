TESTS = ascii.t iso.t utf8.t utf16.t misc.t

# GB18030 test is not working on Cygwin. Cygwin does not support zh_CN.GB18030 locale.
ifeq ($(wildcard ../dos2unix.exe),)
TESTS += gb18030.t
endif

ifneq ($(findstring MINGW,$(shell uname)),MINGW)
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
