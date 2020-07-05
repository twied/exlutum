# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

add_executable(
	arabilis_cpp
	arabilis.cpp
	io.cpp
	io.h
)

do_test(arabilis_cpp io.arabilis)
