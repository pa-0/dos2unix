include dos2unix/version.mk
include unix2dos/version.mk

help:
	@echo "${MAKE} dist         : Create source code distribution packages."
	@echo "${MAKE} tag-dos2unix : Create a tag copy of trunk/dos2unix."
	@echo "${MAKE} tag-unix2dos : Create a tag copy of trunk/unix2dos."
	@echo "${MAKE} tag          : Create a tag copy of trunk/dos2unix and trunk/unix2dos."

# Documentation files are created and included in the source
# package for users that don't have the proper tools installed.
# Documentation creation only works properly on Cygwin or
# a modern Linux distribution.

# .po language files can be older than the .pot files, but
# still be up-to-date. 'msgmerge' will be run by 'make', but
# the .po files are not updated by msgmerge. For users that
# don't have 'gettext' installed we do a few file touches
# so that make doesn't run the gettext tools.

RELEASE_DIR_DOS2UNIX = dos2unix-$(DOS2UNIX_VERSION)
RELEASE_DIR_UNIX2DOS = unix2dos-$(UNIX2DOS_VERSION)

dist-dos2unix:
	rm -rf ../${RELEASE_DIR_DOS2UNIX}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix ../${RELEASE_DIR_DOS2UNIX}
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) mofiles docfiles
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*.pot
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*.po
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*.mo
	cd .. ; tar cvzf ${RELEASE_DIR_DOS2UNIX}.tar.gz ${RELEASE_DIR_DOS2UNIX}

dist-unix2dos:
	rm -rf ../${RELEASE_DIR_UNIX2DOS}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/unix2dos ../${RELEASE_DIR_UNIX2DOS}
	cd ../${RELEASE_DIR_UNIX2DOS} ; $(MAKE) mofiles docfiles
	sleep 2
	cd ../${RELEASE_DIR_UNIX2DOS} ; touch po/*.pot
	sleep 2
	cd ../${RELEASE_DIR_UNIX2DOS} ; touch po/*.po
	sleep 2
	cd ../${RELEASE_DIR_UNIX2DOS} ; touch po/*.mo
	cd .. ; tar cvzf ${RELEASE_DIR_UNIX2DOS}.tar.gz ${RELEASE_DIR_UNIX2DOS}

dist: dist-dos2unix dist-unix2dos


tag-dos2unix:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/dos2unix-${DOS2UNIX_VERSION} \
	    -m "Tagging dos2unix release ${DOS2UNIX_VERSION}."

tag-unix2dos:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/unix2dos \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/unix2dos-${UNIX2DOS_VERSION} \
	    -m "Tagging unix2dos release ${UNIX2DOS_VERSION}."

tag: tag-dos2unix tag-unix2dos

