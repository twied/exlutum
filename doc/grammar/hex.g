// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

// Grammar for the "hex" language, in "antlr-3", see http://www.antlr3.org/

grammar hex;

file
        : item* EOF
        ;

item
        : HEXPAIR
        ;

HEXPAIR
        : ('0'..'9' | 'A'..'F') ('0'..'9' | 'A'..'F')
        ;

WHITESPACE
        : ( ' ' | '\t' | '\n' ) {$channel=HIDDEN;}
        ;

COMMENT
        : '#' (~'\n')* (EOF | '\n')  {$channel=HIDDEN;}
        ;
