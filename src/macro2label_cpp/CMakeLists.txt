# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	macro2label_cpp
	macro2label.cpp
)

do_test(macro2label_cpp invalid_macro_name.macro2label)
do_test(macro2label_cpp label_in_macro.macro2label)
do_test(macro2label_cpp macro_in_comment.macro2label)
do_test(macro2label_cpp macro_in_macro.macro2label)
do_test(macro2label_cpp macros.macro2label)
do_test(macro2label_cpp missing_colon.macro2label)
do_test(macro2label_cpp missing_quote1.macro2label)
do_test(macro2label_cpp missing_quote2.macro2label)
do_test(macro2label_cpp missing_space.macro2label)
