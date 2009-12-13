
prefix=c:/usr/local
LDFLAGS_EXTRA = -lintl -liconv

all:
	$(MAKE) all EXE=.exe LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"


install:
	$(MAKE) install EXE=.exe LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix)

clean:
	$(MAKE) clean EXE=.exe prefix=$(prefix)

