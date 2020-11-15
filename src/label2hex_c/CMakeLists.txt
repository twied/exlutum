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

do_test(label2hex_c labels.label2hex)
do_test(label2hex_c label_empty.label2hex)
do_test(label2hex_c label_eof.label2hex)
do_test(label2hex_c label_invalid_char.label2hex)
do_test(label2hex_c label_invalid_cont.label2hex)
do_test(label2hex_c label_invalid_start.label2hex)
do_test(label2hex_c label_too_long.label2hex)
do_test(label2hex_c oddnybbles_ok.label2hex)
do_test(label2hex_c size.label2hex)
do_test(label2hex_c undefined_label.label2hex)
