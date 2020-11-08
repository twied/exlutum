// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    while(1) {
        int c1 = getchar();

        if (c1 == EOF) {
            exit(0);
        } else if (c1 == '#') {
            while (1) {
                int c = getchar();

                if (c == EOF) {
                    exit(0);
                }

                if (c == '\n') {
                    break;
                }
            }
            continue;
        } else if (c1 == ' ' || c1 == '\t' || c1 == '\n') {
            continue;
        } else if (c1 >= '0' && c1 <= '9') {
            c1 -= '0';
        } else if (c1 >= 'A' && c1 <= 'F') {
            c1 -= 'A' - 0x0A;
        } else {
            exit(1);
        }

        int c2 = getchar();
        if (c2 >= '0' && c2 <= '9') {
            c2 -= '0';
        } else if (c2 >= 'A' && c2 <= 'F') {
            c2 -= 'A' - 0x0A;
        } else {
            exit(1);
        }

        putchar(c1 * 16 + c2);
    }
}

