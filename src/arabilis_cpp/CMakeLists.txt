# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	arabilis_cpp
	arabilis.cpp
	ast.cpp
	ast.h
	backend.cpp
	backend.h
	frontend.cpp
	frontend.h
	io.cpp
	io.h
)

do_test(arabilis_cpp io.arabilis)
do_test(arabilis_cpp lex.arabilis)
do_test(arabilis_cpp lex_invalid_escape.arabilis)
do_test(arabilis_cpp lex_unknown_escape.arabilis)
do_test(arabilis_cpp lex_unknown_token.arabilis)
do_test(arabilis_cpp lex_unterminated_string.arabilis)
do_test(arabilis_cpp parse.arabilis)
do_test(arabilis_cpp parse_for_name_mismatch.arabilis)
do_test(arabilis_cpp parse_unexpected_token.arabilis)
do_test(arabilis_cpp parse_break_outside_loop.arabilis)
do_test(arabilis_cpp parse_continue_outside_loop.arabilis)
do_test(arabilis_cpp parse_duplicate_global.arabilis)
do_test(arabilis_cpp parse_duplicate_local.arabilis)
do_test(arabilis_cpp parse_missing_main.arabilis)
do_test(arabilis_cpp parse_unknown_symbol.arabilis)
