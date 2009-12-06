include dos2unix/version.mk
include unix2dos/version.mk

help:
	@echo "${MAKE} dist         : Create source code distribution packages."
	@echo "${MAKE} tag-dos2unix : Create a tag copy of trunk/dos2unix."
	@echo "${MAKE} tag-unix2dos : Create a tag copy of trunk/unix2dos."
	@echo "${MAKE} tag          : Create a tag copy of trunk/dos2unix and trunk/unix2dos."


RELEASE_DIR_DOS2UNIX = ../dos2unix-$(DOS2UNIX_VERSION)
RELEASE_DIR_UNIX2DOS = ../unix2dos-$(UNIX2DOS_VERSION)


dist:
	rm -rf ${RELEASE_DIR_DOS2UNIX}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix ${RELEASE_DIR_DOS2UNIX}
	cd .. ; tar cvzf dos2unix-${DOS2UNIX_VERSION}.tar.gz dos2unix-${DOS2UNIX_VERSION}
	rm -rf ${RELEASE_DIR_UNIX2DOS}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/unix2dos ${RELEASE_DIR_UNIX2DOS}
	cd .. ; tar cvzf unix2dos-${UNIX2DOS_VERSION}.tar.gz unix2dos-${UNIX2DOS_VERSION}


tag-dos2unix:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/dos2unix-${DOS2UNIX_VERSION} \
	    -m "Tagging dos2unix release ${DOS2UNIX_VERSION}."

tag-unix2dos:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/unix2dos \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/unix2dos-${UNIX2DOS_VERSION} \
	    -m "Tagging unix2dos release ${UNIX2DOS_VERSION}."

tag: tag-dos2unix tag-unix2dos

