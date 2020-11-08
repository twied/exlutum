# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	label2hex_cpp
	label2hex.cpp
)

do_test(label2hex_cpp labels.label2hex)
do_test(label2hex_cpp label_empty.label2hex)
do_test(label2hex_cpp label_eof.label2hex)
do_test(label2hex_cpp label_invalid_char.label2hex)
do_test(label2hex_cpp label_invalid_cont.label2hex)
do_test(label2hex_cpp label_invalid_start.label2hex)
do_test(label2hex_cpp label_too_long.label2hex)
do_test(label2hex_cpp oddnybbles_ok.label2hex)
do_test(label2hex_cpp size.label2hex)
do_test(label2hex_cpp undefined_label.label2hex)
