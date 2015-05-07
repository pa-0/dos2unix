Although locale -a says on FreeBSD 10.1 that zh_CN.GB18030 is supported,
the conversion of CJK characters fails.
This makes the gb18030 test of dos2unix fail on FreeBSD.


The problem is wcstombs(), and this is demonstrated with wcstombs_test.c.

Run the test with the command: gmake wcstombs



