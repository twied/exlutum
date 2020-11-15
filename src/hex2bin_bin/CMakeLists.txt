# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_custom_target(hex2bin_bin ALL)

add_custom_command(
	TARGET hex2bin_bin
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hex2bin ${CMAKE_CURRENT_BINARY_DIR}/hex2bin_bin
)

do_test(hex2bin_bin comments.hex2bin)
do_test(hex2bin_bin empty.hex2bin)
do_test(hex2bin_bin helloworld.hex2bin)
do_test(hex2bin_bin lowercase.hex2bin)
do_test(hex2bin_bin newlines.hex2bin)
do_test(hex2bin_bin oddnybbles.hex2bin)
