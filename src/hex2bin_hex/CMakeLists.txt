# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_custom_target(hex2bin_hex ALL)

add_custom_command(
	TARGET hex2bin_hex
	COMMAND ${CMAKE_BINARY_DIR}/src/${EXLUTUM_HEX2BIN}/${EXLUTUM_HEX2BIN} < ${CMAKE_CURRENT_SOURCE_DIR}/hex2bin.hex > ${CMAKE_CURRENT_BINARY_DIR}/hex2bin_hex
	COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/hex2bin_hex
)

add_dependencies(
	hex2bin_hex
	${EXLUTUM_HEX2BIN}
)

do_test(hex2bin_hex comments.hex2bin)
do_test(hex2bin_hex empty.hex2bin)
do_test(hex2bin_hex helloworld.hex2bin)
do_test(hex2bin_hex lowercase.hex2bin)
do_test(hex2bin_hex newlines.hex2bin)
do_test(hex2bin_hex oddnybbles.hex2bin)
