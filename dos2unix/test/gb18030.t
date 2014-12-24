#!/usr/bin/perl

# Requires perl-Test-Simple installation.
use Test::Simple tests => 9;

$suffix = "";
if (-e "../dos2unix.exe") {
  $suffix = ".exe";
}
$DOS2UNIX = "../dos2unix" . $suffix;
$MAC2UNIX = "../mac2unix" . $suffix;
$UNIX2DOS = "../unix2dos" . $suffix;
$UNIX2MAC = "../unix2mac" . $suffix;

$ENV{'LC_ALL'} = 'zh_CN.GB18030';

system("$DOS2UNIX -v -n dos_gb.txt out_unix.txt; cmp out_unix.txt unix.txt");
ok( $? == 0, 'dos2unix removes GB18030 BOM' );

system("$DOS2UNIX -v -b -n dos_bom.txt out_unix.txt; cmp out_unix.txt unix_bom.txt");
ok( $? == 0, 'dos2unix -b keeps UTF-8 BOM in GB18030 locale' );

system("$DOS2UNIX -v -m -n dos.txt out_unix.txt; cmp out_unix.txt unix_gb.txt");
ok( $? == 0, 'dos2unix -m adds GB18030 BOM' );

system("$UNIX2DOS -v -n unix_bom.txt out_dos.txt; cmp out_dos.txt dos_bom.txt");
ok( $? == 0, 'unix2dos keeps UTF-8 BOM in GB18030 locale' );

system("$UNIX2DOS -v -r -n unix_gb.txt out_dos.txt; cmp out_dos.txt dos.txt");
ok( $? == 0, 'unix2dos -r removes GB18030 BOM' );

system("$UNIX2DOS -v -m -n unix.txt out_dos.txt; cmp out_dos.txt dos_gb.txt");
ok( $? == 0, 'unix2dos -m adds GB18030 BOM' );

system("$DOS2UNIX -v -n utf16le.txt out_unix.txt; cmp out_unix.txt gb18030u.txt");
ok( $? == 0, 'dos2unix convert DOS UTF-16 to Unix GB18030' );

$ENV{'LC_ALL'} = 'en_US.UTF-8';

system("$DOS2UNIX -v -b -n dos_gb.txt out_unix.txt; cmp out_unix.txt unix_gb.txt");
ok( $? == 0, 'dos2unix -b keeps GB18030 BOM in UTF-8 locale' );

system("$UNIX2DOS -v -n unix_gb.txt out_dos.txt; cmp out_dos.txt dos_gb.txt");
ok( $? == 0, 'unix2dos keeps GB18030 BOM in UTF-8 locale' );

