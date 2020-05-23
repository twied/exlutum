# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	label2hex_c
	label2hex.c
)

# Build as 32 bit binary, as syscalls are much easier to handle that way.
set_target_properties(
	label2hex_c PROPERTIES
	COMPILE_OPTIONS "-m32"
	LINK_FLAGS "-m32"
)
