// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

// Grammar for the "arabilis" language, in "antlr-3", see http://www.antlr3.org/

grammar arabilis;

file
        : ( global_var | function )+ EOF
        ;

global_var
        : 'var' IDENTIFIER '=' (INT | '-' INT | STRING | 'true' | 'false') ';'
        ;

function
        : 'function' IDENTIFIER '(' ( IDENTIFIER ( ',' IDENTIFIER )* )? ')' '{' statement* '}'
        ;

statement
        : break_statement
        | continue_statement
        | expression_statement
        | for_statement
        | if_statement
        | let_statement
        | return_statement
        | var_statement
        | while_statement
        ;

break_statement
        : 'break' ';'
        ;

continue_statement
        : 'continue' ';'
        ;

expression_statement
        : expression ';'
        ;

for_statement
        : 'for' '(' 'var' IDENTIFIER '=' expression ';' expression ';' 'let' IDENTIFIER '=' expression ')' '{' statement* '}'
        ;

if_statement
        : 'if' '(' expression ')' '{' statement* '}' ( 'else' '{' statement* '}' )?
        ;

let_statement
        : 'let' IDENTIFIER '=' expression ';'
        ;

return_statement
        : 'return' (expression)? ';'
        ;

var_statement
        : 'var' IDENTIFIER ('=' expression)? ';'
        ;

while_statement
        : 'while' '(' expression ')' '{' statement* '}'
        ;


expression
        : term ( ( '+' | '-' | '*' | '/' | '%' | '&&' | '||' | '&' | '|' | '^' | '==' | '!=' | '<' | '<=' | '>' | '>=' ) term )?
        ;

term
        : ( '+' | '-' | '!' | '~' )? factor
        ;

factor
        : IDENTIFIER                                                    /* variable */
        | IDENTIFIER '(' ( expression ( ',' expression )* )? ')'        /* function call */
        | STRING                                                        /* ptr to a c string */
        | 'true'                                                        /* alias for 1 */
        | 'false'                                                       /* alias for 0 */
        | INT                                                           /* integer value */
        | '&' IDENTIFIER                                                /* addressof operator */
        | '(' expression ')'
        ;

IDENTIFIER
        : ( 'a'..'z' | 'A'..'Z' | '_' ) ( 'a'..'z' | 'A'..'Z' | '0'..'9' | '_' )*
        ;

INT
        : ( '0' .. '9' )+
        ;

STRING
        : '"' ( '\\' ( 't' | 'n' | 'r' | '"' | '\\' | 'x' ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' ) ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' ) ) | ~( '\\' | '"' ) )* '"'
        ;

COMMENT
        : '#' ~( '\n'|'\r' )* '\r'? '\n' { $channel=HIDDEN; }
        ;

WHITESPACE
        : ( ' ' | '\t' | '\r' | '\n' ) { $channel=HIDDEN; }
        ;
