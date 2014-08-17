all: test

check: test

test:
	prove -v iso.t utf8.t utf16.t

iso:
	prove -v $@.t

utf8:
	prove -v $@.t

utf16:
	prove -v $@.t

clean:
	rm -f out*.txt
