# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

function(do_test program_under_test fixture)
	add_test(
		NAME runtest_${program_under_test}_${fixture}
		COMMAND ${CMAKE_SOURCE_DIR}/tests/runtest.py ${program_under_test} ${CMAKE_SOURCE_DIR}/tests/${fixture}.yaml
	)
endfunction(do_test)

add_subdirectory(hex2bin_c)

add_subdirectory(label2hex_cpp)
