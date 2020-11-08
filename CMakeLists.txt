# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

cmake_minimum_required(VERSION 3.10)

project(
	"Ex Lutum"
	VERSION 1.0
	DESCRIPTION "Self-hosting compiler from dirt."
	LANGUAGES CXX C ASM_NASM
)

add_subdirectory(src)
