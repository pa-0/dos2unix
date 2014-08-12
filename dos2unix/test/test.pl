#!/usr/bin/perl

# Requires perl-Test-Simple installation.
use Test::More tests => 4;

$DOS2UNIX = "../dos2unix";
$MAC2UNIX = "../mac2unix";
$UNIX2DOS = "../unix2dos";
$UNIX2MAC = "../unix2mac";

system("$DOS2UNIX -v -n dos.txt out_unix.txt; cmp out_unix.txt unix.txt");
ok( $? == 0, 'DOS to Unix conversion' );

system("$MAC2UNIX -v -n mac.txt out_unix.txt; cmp out_unix.txt unix.txt");
ok( $? == 0, 'DOS to Unix conversion' );

system("$UNIX2DOS -v -n unix.txt out_dos.txt; cmp out_dos.txt dos.txt");
ok( $? == 0, 'Unix to DOS conversion' );

system("$UNIX2MAC -v -n unix.txt out_mac.txt; cmp out_mac.txt mac.txt");
ok( $? == 0, 'Unix to Mac conversion' );
