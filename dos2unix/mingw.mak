
prefix=c:/usr/local
LDFLAGS_EXTRA = -lintl -liconv

all:
	$(MAKE) all EXE=.exe LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix)


install:
	$(MAKE) install EXE=.exe LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix)
