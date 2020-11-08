# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	hex2bin_c
	hex2bin.c
)

do_test(hex2bin_c comments.hex2bin)
do_test(hex2bin_c empty.hex2bin)
do_test(hex2bin_c helloworld.hex2bin)
do_test(hex2bin_c lowercase.hex2bin)
do_test(hex2bin_c newlines.hex2bin)
do_test(hex2bin_c oddnybbles.hex2bin)
