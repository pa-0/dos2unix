
prefix=c:/djgpp
ENABLE_NLS=
LDFLAGS_EXTRA = -lintl -liconv

all:
	$(MAKE) all EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"


install:
	$(MAKE) install EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix)

clean:
	$(MAKE) clean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

