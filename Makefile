include dos2unix/version.mk

GITREPO = git://git.code.sf.net/p/dos2unix/dos2unix

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


# Target: dist - Create source packages for distribution.
dist:
	rm -rf ../clone
	rm -rf ../${RELEASE_DIR_DOS2UNIX}
	rm -rf ../${RELEASE_DIR_D2U}
	git clone ${GITREPO} ../clone
	mv ../clone/dos2unix ../${RELEASE_DIR_DOS2UNIX}
	rm -rf ../clone
	# Include doc files, to make it easier to build dos2unix.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) man txt html
	# Make sure .po files are up to date.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) merge
	# cleanup.
	cd ../${RELEASE_DIR_DOS2UNIX} ; $(MAKE) mostlyclean
	# fix some file mods.
	cd ../${RELEASE_DIR_DOS2UNIX} ; chmod -x Makefile *.txt *.c *.h *.mak *.mk po/*.* man/*/man1/*.*
	# Create DOS source package.
	cd .. ; cp -Rp $(RELEASE_DIR_DOS2UNIX) $(RELEASE_DIR_D2U)
	# DJGPP's GNU make 3.79 has trouble with makefiles in DOS format.
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate w*.mak *.txt *.c *.h
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate --add-bom ChangeLog.txt README.txt
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate --add-bom man/man1/*.txt  man/*/man1/*.txt
	cd ../$(RELEASE_DIR_D2U) ; unix2dos --keepdate man/man1/*.htm* man/*/man1/*.htm*
	# Create the package.
	cd .. ; tar cvzf ${RELEASE_DIR_DOS2UNIX}.tar.gz ${RELEASE_DIR_DOS2UNIX}
	cd .. ; rm -f ${RELEASE_DIR_D2U}.zip
	cd .. ; zip -r ${RELEASE_DIR_D2U}.zip ${RELEASE_DIR_D2U}

# Create package from local git repo.
distlocal:
	$(MAKE) dist GITREPO=${PWD}


# Create pgp signature. Required for Debian Linux.
# See http://narfation.org/2013/06/23/signed-upstream-tarballs-in-debian
pgpsign:
	cd ..; gpg --detach-sign --armor ${RELEASE_DIR_DOS2UNIX}.tar.gz

# Send the key tp HKP/SKS keyserver.
# You might want to add this to receive kes easily:
#
#    # .gnupg/gpg.conf
#    keyserver  hkp://pool.sks-keyservers.net
pgpsend:
	cd ..; gpg --keyserver pool.sks-keyservers.net --send-keys B12725BE

# Target: tag - Create a tag
tag:
	git tag -a dos2unix-${DOS2UNIX_VERSION} -m "Tagging dos2unix release ${DOS2UNIX_VERSION}."
	git push origin dos2unix-${DOS2UNIX_VERSION}

