// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

// Grammar for the "macro" language, in "antlr-3", see http://www.antlr3.org/

grammar macro;

file
        : item* EOF
        ;

item
        : HEXPAIR
        | macro_declaration
        | label_declaration
        | label_insertion
        ;

macro_declaration
        : '%' IDENTIFIER ':' '"' (~'"')* '"'
        ;

label_declaration
        : '.' IDENTIFIER ':'
        ;

label_insertion
        : IDENTIFIER
        ;

HEXPAIR
        : ('0'..'9' | 'A'..'F') ('0'..'9' | 'A'..'F')
        ;

IDENTIFIER
        : ('a'..'z') ('a'..'z' | '0'..'9' | '_')*
        ;

WHITESPACE
        : ( ' ' | '\t' | '\n' ) {$channel=HIDDEN;}
        ;

COMMENT
        : '#' (~'\n')* (EOF | '\n')  {$channel=HIDDEN;}
        ;
