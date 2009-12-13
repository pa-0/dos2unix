
prefix=c:/usr/local
LDFLAGS_EXTRA = -lintl -liconv

all:
	$(MAKE) all EXE=.exe LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix)


install:
	$(MAKE) install EXE=.exe GCCFLAGS="-O2 -Wall -std=gnu99" LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix)

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix)

clean:
	$(MAKE) clean EXE=.exe prefix=$(prefix)

