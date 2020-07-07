// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#ifndef AST_H_
#define AST_H_

namespace arabilis {

enum class Token {
    string_break,
    string_continue,
    string_else,
    string_false,
    string_for,
    string_function,
    string_if,
    string_let,
    string_return,
    string_true,
    string_var,
    string_while,

    bracket_round_left,
    bracket_round_right,
    bracket_curly_left,
    bracket_curly_right,

    token_plus,
    token_minus,
    token_multiply,
    token_divide,
    token_modulo,

    token_log_not,
    token_log_and,
    token_log_or,
    token_bit_not,
    token_bit_and,
    token_bit_or,
    token_bit_xor,

    token_equal,
    token_notequal,
    token_less,
    token_lessequal,
    token_greater,
    token_greaterequal,

    token_comma,
    token_semicolon,
    token_assign,

    identifier,
    numeral,
    literal,
    eof
};

const char* token_to_name(Token token);

} /* namespace arabilis */

#endif /* AST_H_ */
