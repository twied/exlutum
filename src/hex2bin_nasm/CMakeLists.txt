# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_custom_target(hex2bin_nasm ALL)

# not a simple "add_executable()" as we build the ELF-header ourselves.
add_custom_command(
	TARGET hex2bin_nasm
	COMMAND nasm ${CMAKE_CURRENT_SOURCE_DIR}/hex2bin.s -o ${CMAKE_CURRENT_BINARY_DIR}/hex2bin_nasm
	COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/hex2bin_nasm
)
