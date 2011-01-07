#ifndef __DOS2UNIX_H
#define __DOS2UNIX_H

/*
 *  Copyright (C) 2009-2011 Erwin Waterlander
 *  Copyright (C) 1994-1995 Benjamin Lin.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice in the documentation and/or other materials provided with
 *     the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __GNUC__
#ifndef strcmpi
#  define strcmpi(s1, s2) strcasecmp(s1, s2)
#endif
#endif

/* ASCII mode. No conversion. */

static int D2UAsciiTable[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};


/* 7bit mode. */
/* All 8 bit non-ASCII characters are converted to a space (\x20) */

static int D2U7BitTable[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
  '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20',
};


/* CP437 -> ISO-8859-1 */
/* Non-convertable characters are converted to a dot (\x2e) */

static int D2UIso437Table[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\xc7', '\xfc', '\xe9', '\xe2', '\xe4', '\xe0', '\xe5', '\xe7', '\xea', '\xeb', '\xe8', '\xef', '\xee', '\xec', '\xc4', '\xc5',
  '\xc9', '\xe6', '\xc6', '\xf4', '\xf6', '\xf2', '\xfb', '\xf9', '\xff', '\xd6', '\xdc', '\xa2', '\xa3', '\xa5', '\x2e', '\x2e',
  '\xe1', '\xed', '\xf3', '\xfa', '\xf1', '\xd1', '\xaa', '\xba', '\xbf', '\x2e', '\xac', '\xbd', '\xbc', '\xa1', '\xab', '\xbb',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\x2e', '\xdf', '\x2e', '\x2e', '\x2e', '\x2e', '\xb5', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\x2e', '\xb1', '\x2e', '\x2e', '\x2e', '\x2e', '\xf7', '\x2e', '\xb0', '\x2e', '\xb7', '\x2e', '\x2e', '\xb2', '\x2e', '\xa0',
};

/* CP850 -> ISO-8859-1 */
/* Non-convertable characters are converted to a dot (\x2e) */
// 9b>f8, 9d>d8, 9e>d7
// a9>ae
// b5>c1, b6>c2, b6>c2, b7>c0, b8>a9, bd>a2, be>a5
// c6>e3, c7>c3, cf>a4
// d0>f0, d1>d0, d2>ca, d3>cb, d4>c8, d6>cd, d7>ce, d8>cf, dd>a6, de>cc
// e0>d3, e2>d4, e3>d2, e4>f5, e5>d5, e7>fe, e8>de, e9>da, ea>db, eb>d9, ec>fd, ed>dd, ee>af, ef>b4
// f0>ad, f3>be, f4>b6, f5>a7, f7>b8, f9>a8, fb>b9, fc>b3, fd>b2, ff>a0
static int D2UIso850Table[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\xc7', '\xfc', '\xe9', '\xe2', '\xe4', '\xe0', '\xe5', '\xe7', '\xea', '\xeb', '\xe8', '\xef', '\xee', '\xec', '\xc4', '\xc5',
  '\xc9', '\xe6', '\xc6', '\xf4', '\xf6', '\xf2', '\xfb', '\xf9', '\xff', '\xd6', '\xdc', '\xf8', '\xa3', '\xd8', '\xd7', '\x2e',
  '\xe1', '\xed', '\xf3', '\xfa', '\xf1', '\xd1', '\xaa', '\xba', '\xbf', '\xae', '\xac', '\xbd', '\xbc', '\xa1', '\xab', '\xbb',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\xc1', '\xc2', '\xc0', '\xa9', '\x2e', '\x2e', '\x2e', '\x2e', '\xa2', '\xa5', '\x2e',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\xe3', '\xc3', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\xa4',
  '\xf0', '\xd0', '\xca', '\xcb', '\xc8', '\x2e', '\xcd', '\xce', '\xcf', '\x2e', '\x2e', '\x2e', '\x2e', '\xa6', '\xcc', '\x2e',
  '\xd3', '\xdf', '\xd4', '\xd2', '\xf5', '\xd5', '\xb5', '\xfe', '\xde', '\xda', '\xdb', '\xd9', '\xfd', '\xdd', '\xaf', '\xb4',
  '\xad', '\xb1', '\x2e', '\xbe', '\xb6', '\xa7', '\xf7', '\xb8', '\xb0', '\xa8', '\xb7', '\xb9', '\xb3', '\xb2', '\x2e', '\xa0',
};

/* CP1252 -> ISO-8859-1 */
/* Non-convertable characters are converted to a dot (\x2e) */

static int D2UIso1252Table[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e', '\x2e',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

#ifdef ENABLE_NLS

#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#else

#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif


#endif
