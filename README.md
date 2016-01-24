DOS2UNIX
========

DOS/Mac to Unix and vice versa text file format converter.

Description
-----------

The Dos2unix package includes utilities dos2unix and unix2dos to convert plain
text files in DOS or Mac format to Unix format and vice versa.

In DOS/Windows text files a line break, also known as newline, is a combination
of two characters: a Carriage Return (CR) followed by a Line Feed (LF). In Unix
text files a line break is a single character: the Line Feed (LF). In Mac text
files, prior to Mac OS X, a line break was single Carriage Return (CR)
character. Nowadays Mac OS uses Unix style (LF) line breaks.

Besides line breaks Dos2unix can also convert the encoding of files. A few DOS
code pages can be converted to Unix Latin-1. And Windows Unicode (UTF-16) files
can be converted to Unix Unicode (UTF-8) files.

Binary files are automatically skipped, unless conversion is forced.

Non-regular files, such as directories and FIFOs, are automatically skipped.

Symbolic links and their targets are by default kept untouched. Symbolic links
can optionally be replaced, or the output can be written to the symbolic link
target. Writing to a symbolic link target is not supported on Windows.

Dos2unix was modelled after dos2unix under SunOS/Solaris.  There is one
important difference with the original SunOS/Solaris version. This version does
by default in-place conversion (old file mode), while the original
SunOS/Solaris version only supports paired conversion (new file mode). See also
options "-o" and "-n". Another difference is that the SunOS/Solaris version
uses by default iso mode conversion while this version uses by default ascii
mode conversion.

Features
--------

* Native language support.
* Automatically skips binary and non-regular files.
* In-place, paired, or stdio mode conversion.
* Keep original file dates option.
* 7-bit and iso conversion modes like SunOS dos2unix.
* Conversion of Unicode UTF-16 files.
* Handles Unicode Byte Order Mark (BOM).
* Display file information.
* Secure.


Project information
-------------------

Maintainer: Erwin Waterlander <waterlan@xs4all.nl>
Git: git clone git://git.code.sf.net/p/dos2unix/dos2unix
[Home page](http://waterlan.home.xs4all.nl/dos2unix.html)
[SourceForge](http://sourceforge.net/projects/dos2unix/)
[ChangeLog](dos2unix/ChangeLog.txt)
[NEWS](dos2unix/NEWS.txt)

