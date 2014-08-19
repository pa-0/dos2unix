all: test

check: test

test:
	prove -v ascii.t iso.t utf8.t utf16.t misc.t

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

clean:
	rm -f out*.txt
