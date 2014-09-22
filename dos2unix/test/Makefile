TESTS = ascii.t iso.t utf8.t utf16.t misc.t

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

symlink:
	prove -v $@.t

clean:
	rm -f out*.txt in_link.txt
