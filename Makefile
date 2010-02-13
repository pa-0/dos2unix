include dos2unix/version.mk

help:
	@echo "${MAKE} dist         : Create source code distribution packages."
	@echo "${MAKE} tag          : Create a tag copy of trunk."

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

dist:
	rm -rf ../${RELEASE_DIR_DOS2UNIX}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix ../${RELEASE_DIR_DOS2UNIX}
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) mofiles docfiles
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*/*.pot
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*/*.po
	sleep 2
	cd ../${RELEASE_DIR_DOS2UNIX} ; touch po/*/*.mo
	cd .. ; tar cvzf ${RELEASE_DIR_DOS2UNIX}.tar.gz ${RELEASE_DIR_DOS2UNIX}

tag:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/dos2unix-${DOS2UNIX_VERSION} \
	    -m "Tagging dos2unix release ${DOS2UNIX_VERSION}."


