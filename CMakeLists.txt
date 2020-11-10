# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

cmake_minimum_required(VERSION 3.10)

project(
	"Ex Lutum"
	VERSION 1.0
	DESCRIPTION "Self-hosting compiler from dirt."
	LANGUAGES CXX C ASM_NASM
)

set(EXLUTUM_HEX2BIN "hex2bin_c" CACHE STRING "hex2bin compiler")
set(EXLUTUM_LABEL2HEX "label2hex_cpp" CACHE STRING "label2hex compiler")
set(EXLUTUM_MACRO2LABEL "macro2label_cpp" CACHE STRING "macro2label compiler")
set(EXLUTUM_ARABILIS "arabilis_cpp" CACHE STRING "arabilis compiler")

set_property(
	CACHE EXLUTUM_HEX2BIN PROPERTY STRINGS
	"hex2bin_c"
)

set_property(
	CACHE EXLUTUM_LABEL2HEX PROPERTY STRINGS
	"label2hex_cpp"
)

set_property(
	CACHE EXLUTUM_MACRO2LABEL PROPERTY STRINGS
	"macro2label_cpp"
)

set_property(
	CACHE EXLUTUM_ARABILIS PROPERTY STRINGS
	"arabilis_cpp"
)

enable_testing()

add_subdirectory(src)
add_subdirectory(examples)
