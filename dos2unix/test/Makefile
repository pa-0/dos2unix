EXE=
ifeq ($(findstring CYGWIN,$(shell uname)),CYGWIN)
	EXE = .exe
endif
ifeq ($(findstring MSYS,$(shell uname)),MSYS)
	EXE = .exe
endif
ifeq ($(findstring MINGW32,$(shell uname)),MINGW32)
	EXE = .exe
endif
ifneq ($(DJGPP),)
	EXE = .exe
endif
ifeq ($(shell uname),OS/2)
	EXE = .exe
endif


DOS2UNIX=../dos2unix$(EXE)
MAC2UNIX=../mac2unix$(EXE)
UNIX2DOS=../unix2dos$(EXE)
UNIX2MAC=../unix2mac$(EXE)

all: test

check: test

test: iso utf16

iso:
	# DOS to Unix conversion
	$(DOS2UNIX) -v -n dos.txt out_unix.txt; cmp out_unix.txt unix.txt
	# DOS to Unix conversion
	$(MAC2UNIX) -v -n mac.txt out_unix.txt; cmp out_unix.txt unix.txt
	# Unix to DOS conversion
	$(UNIX2DOS) -v -n unix.txt out_dos.txt; cmp out_dos.txt dos.txt
	# Unix to Mac conversion
	$(UNIX2MAC) -v -n unix.txt out_mac.txt; cmp out_mac.txt mac.txt
	@echo ""
	# dos2unix must not change unix line breaks
	$(DOS2UNIX) -v -n unix.txt out_unix.txt; cmp out_unix.txt unix.txt
	# dos2unix must not change mac line breaks
	$(DOS2UNIX) -v -n mac.txt out_unix.txt; cmp out_unix.txt mac.txt
	# mac2unix must not change unix line breaks
	$(MAC2UNIX) -v -n unix.txt out_unix.txt; cmp out_unix.txt unix.txt
	# mac2unix must not change dos line breaks
	$(MAC2UNIX) -v -n dos.txt out_unix.txt; cmp out_unix.txt dos.txt
	# unix2dos must not change dos line breaks
	$(UNIX2DOS) -v -n dos.txt out_dos.txt; cmp out_dos.txt dos.txt
	# unix2dos must not change mac line breaks
	$(UNIX2DOS) -v -n mac.txt out_dos.txt; cmp out_dos.txt mac.txt
	# unix2mac must not change dos line breaks
	$(UNIX2MAC) -v -n dos.txt out_mac.txt; cmp out_mac.txt dos.txt
	# unix2mac must not change mac line breaks
	$(UNIX2MAC) -v -n mac.txt out_mac.txt; cmp out_mac.txt mac.txt
	@echo ""
	# DOS to Unix conversion mixed
	$(DOS2UNIX) -v -n mixed.txt out.txt; cmp out.txt mixedd2u.txt
	# DOS to Unix conversion mixed
	$(MAC2UNIX) -v -n mixed.txt out.txt; cmp out.txt mixedm2u.txt
	# Unix to DOS conversion mixed
	$(UNIX2DOS) -v -n mixed.txt out.txt; cmp out.txt mixedu2d.txt
	# Unix to Mac conversion mixed
	$(UNIX2MAC) -v -n mixed.txt out.txt; cmp out.txt mixedu2m.txt
	@echo ""
	# DOS to Unix conversion with line doubling
	$(DOS2UNIX) -v -l -n dos.txt out_unix.txt; cmp out_unix.txt unix_dbl.txt
	# DOS to Unix conversion with line doubling
	$(MAC2UNIX) -v -l -n mac.txt out_unix.txt; cmp out_unix.txt unix_dbl.txt
	# Unix to DOS conversion with line doubling
	$(UNIX2DOS) -v -l -n unix.txt out_dos.txt; cmp out_dos.txt dos_dbl.txt
	# Unix to Mac conversion with line doubling
	$(UNIX2MAC) -v -l -n unix.txt out_mac.txt; cmp out_mac.txt mac_dbl.txt
	@echo ""
	# dos2unix -l must not change unix line breaks
	$(DOS2UNIX) -v -l -n unix.txt out_unix.txt; cmp out_unix.txt unix.txt
	# dos2unix -l must not change mac line breaks
	$(DOS2UNIX) -v -l -n mac.txt out_unix.txt; cmp out_unix.txt mac.txt
	# mac2unix -l must not change unix line breaks
	$(MAC2UNIX) -v -l -n unix.txt out_unix.txt; cmp out_unix.txt unix.txt
	# mac2unix -l must not change dos line breaks
	$(MAC2UNIX) -v -l -n dos.txt out_unix.txt; cmp out_unix.txt dos.txt
	# unix2dos -l must not change dos line breaks
	$(UNIX2DOS) -v -l -n dos.txt out_dos.txt; cmp out_dos.txt dos.txt
	# unix2dos -l must not change mac line breaks
	$(UNIX2DOS) -v -l -n mac.txt out_dos.txt; cmp out_dos.txt mac.txt
	# unix2mac -l must not change dos line breaks
	$(UNIX2MAC) -v -l -n dos.txt out_mac.txt; cmp out_mac.txt dos.txt
	# unix2mac -l must not change mac line breaks
	$(UNIX2MAC) -v -l -n mac.txt out_mac.txt; cmp out_mac.txt mac.txt
	@echo ""
	# 7bit
	$(DOS2UNIX) -v -7 -n char.txt out_unix.txt; cmp out_unix.txt char7.txt
	@echo ""

utf8:
	# dos2unix removes BOM
	$(DOS2UNIX) -v -n dos_bom.txt out_unix.txt; cmp out_unix.txt unix.txt
	# dos2unix -b keeps BOM
	$(DOS2UNIX) -v -b -n dos_bom.txt out_unix.txt; cmp out_unix.txt unix_bom.txt
	# dos2unix -m adds BOM
	$(DOS2UNIX) -v -m -n dos.txt out_unix.txt; cmp out_unix.txt unix_bom.txt
	# unix2dos keeps BOM
	$(UNIX2DOS) -v -n unix_bom.txt out_dos.txt; cmp out_dos.txt dos_bom.txt
	# unix2dos -r removes BOM
	$(UNIX2DOS) -v -r -n unix_bom.txt out_dos.txt; cmp out_dos.txt dos.txt
	# unix2dos -m adds BOM
	$(UNIX2DOS) -v -m -n unix.txt out_dos.txt; cmp out_dos.txt dos_bom.txt
	@echo ""

utf16:
	# DOS UTF-16LE to Unix UTF-8
	$(DOS2UNIX) -v -n utf16le.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	# DOS UTF-16BE to Unix UTF-8
	$(DOS2UNIX) -v -n utf16be.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	# DOS UTF-16LE to DOS UTF-8
	$(UNIX2DOS) -v -n utf16le.txt out_dos.txt; cmp out_dos.txt utf8dos.txt
	# DOS UTF-16BE to DOS UTF-8
	$(UNIX2DOS) -v -n utf16be.txt out_dos.txt; cmp out_dos.txt utf8dos.txt
	@echo ""
	# UTF-16LE without BOM to UTF-8
	$(DOS2UNIX) -v -ul -n utf16len.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	# UTF-16BE without BOM to UTF-8
	$(DOS2UNIX) -v -ub -n utf16ben.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	# BOM overrides -ul
	$(DOS2UNIX) -v -ul -n utf16be.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	# BOM overrides -ub
	$(DOS2UNIX) -v -ub -n utf16le.txt out_unix.txt; cmp out_unix.txt utf8unix.txt
	@echo ""
	# DOS UTF-16LE to Unix UTF-8, keep BOM
	$(DOS2UNIX) -v -b -n utf16le.txt out_unix.txt; cmp out_unix.txt utf8unxb.txt
	# DOS UTF-16LE to DOS UTF-8, remove BOM
	$(UNIX2DOS) -v -r -n utf16le.txt out_dos.txt; cmp out_dos.txt utf8dosn.txt
	@echo ""
	# mac2unix does not change utf16 DOS line breaks.
	$(MAC2UNIX) -v -n utf16le.txt out_unix.txt; cmp out_unix.txt utf8dosn.txt
	# unix2mac does not change utf16 DOS line breaks.
	$(UNIX2MAC) -v -n utf16le.txt out_mac.txt; cmp out_mac.txt utf8dos.txt
	@echo ""
	# DOS UTF-16LE to DOS UTF-16
	$(UNIX2DOS) -v -u -n utf16le.txt out_dos.txt; cmp out_dos.txt utf16le.txt
	# DOS UTF-16BE to DOS UTF-16
	$(UNIX2DOS) -v -u -n utf16be.txt out_dos.txt; cmp out_dos.txt utf16be.txt
	# DOS UTF-16LE to Unix UTF-16
	$(DOS2UNIX) -v -b -u -n utf16.txt out_unix.txt; cmp out_unix.txt utf16u.txt
	# Mac UTF-16LE to Unix UTF-16
	$(MAC2UNIX) -v -b -u -n utf16m.txt out_unix.txt; cmp out_unix.txt utf16u.txt
	# Unix UTF-16 to DOS UTF-16LE
	$(UNIX2DOS) -v -b -u -n utf16u.txt out_dos.txt; cmp out_dos.txt utf16.txt
	# Unix UTF-16 to Mac UTF-16LE
	$(UNIX2MAC) -v -b -u -n utf16u.txt out_mac.txt; cmp out_mac.txt utf16m.txt
	
clean:
	rm -f out*.txt
