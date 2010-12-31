include dos2unix/version.mk

all: help

# Target: help - Display available make targets
help:
	@awk  '/^# +Target:/ {sub("# +Target: +",""); print}' Makefile | sort

# Documentation files are created and included in the source
# package for users that don't have the proper tools installed.
# Documentation creation only works properly on Cygwin or
# a modern Linux distribution.


RELEASE_DIR_DOS2UNIX = dos2unix-$(DOS2UNIX_VERSION)
RELEASE_DIR_D2U = d2u$(DOS2UNIX_VERSION_SHORT)

# Target: dist - Create source code distribution packages
dist:
	rm -rf ../${RELEASE_DIR_DOS2UNIX}
	svn export https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk/dos2unix ../${RELEASE_DIR_DOS2UNIX}
	# Include doc files, to make it easier to build dos2unix.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) doc
	# Make sure .po files are up to date.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) merge
	# cleanup.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) clean
	# fix pod2man bug
	cd ../${RELEASE_DIR_DOS2UNIX}/man/nl/man1 ; perl -pli -e s/e\\\\\\*:/\\\\[:e]/ dos2unix.1
	# fix some file mods.
	cd ../${RELEASE_DIR_DOS2UNIX} ; chmod -x Makefile *.txt *.c *.h *.mak *.mk po/*/*.* man/nl/man1/*.*
	# Create DOS source package.
	cd .. ; cp -Rp $(RELEASE_DIR_DOS2UNIX) $(RELEASE_DIR_D2U)
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate Makefile *.mak *.mk *.txt *.c *.h *.htm man/man1/* man/*/man1/*
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate po/*/*
	# Create doc package for people who are not able to create it.
	cd .. ; tar cvzf ${RELEASE_DIR_DOS2UNIX}-doc.tar.gz \
		${RELEASE_DIR_DOS2UNIX}/dos2unix.txt \
		${RELEASE_DIR_DOS2UNIX}/dos2unix.ps \
		${RELEASE_DIR_DOS2UNIX}/dos2unix.pdf \
		${RELEASE_DIR_DOS2UNIX}/dos2unix.htm \
		${RELEASE_DIR_DOS2UNIX}/man/man1/*.1 \
		${RELEASE_DIR_DOS2UNIX}/man/*/man1/*.1
	# Remove generated doc files from Unix package.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) maintainer-clean
	# Create the package.
	cd .. ; tar cvzf ${RELEASE_DIR_DOS2UNIX}.tar.gz ${RELEASE_DIR_DOS2UNIX}
	cd .. ; zip -r ${RELEASE_DIR_D2U}s.zip ${RELEASE_DIR_D2U}

# Target: tag - Create a tag copy of trunk
tag:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/tags/dos2unix-${DOS2UNIX_VERSION} \
	    -m "Tagging dos2unix release ${DOS2UNIX_VERSION}."

BRANCH=dos2unix-60

branch:
	svn copy https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk \
	         https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/branches/${BRANCH} \
	    -m "Branching ${BRANCH}."


# Get latest changes of trunk into branch.
merge_from_trunk:
	svn merge https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/trunk

# Merge branch into trunk.
# After this the branch is unusable for further work.
# Copy a new branch if needed.
merge_to_trunk:
	svn merge --reintegrate https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/branches/${BRANCH}

# Delete branch
delete_branch:
	svn delete https://dos2unix.svn.sourceforge.net/svnroot/dos2unix/branches/${BRANCH} -m "Delete branch ${BRANCH}"


