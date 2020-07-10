// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "ast.h"

namespace arabilis {

const char* token_to_name(Token t) {
    switch (t) {
    case Token::string_break:
        return "break";
    case Token::string_continue:
        return "continue";
    case Token::string_else:
        return "else";
    case Token::string_false:
        return "false";
    case Token::string_for:
        return "for";
    case Token::string_function:
        return "function";
    case Token::string_if:
        return "if";
    case Token::string_let:
        return "let";
    case Token::string_return:
        return "return";
    case Token::string_true:
        return "true";
    case Token::string_var:
        return "var";
    case Token::string_while:
        return "while";
    case Token::bracket_round_left:
        return "(";
    case Token::bracket_round_right:
        return ")";
    case Token::bracket_curly_left:
        return "{";
    case Token::bracket_curly_right:
        return "}";
    case Token::token_plus:
        return "+";
    case Token::token_minus:
        return "-";
    case Token::token_multiply:
        return "*";
    case Token::token_divide:
        return "/";
    case Token::token_modulo:
        return "%";
    case Token::token_log_not:
        return "!";
    case Token::token_log_and:
        return "&&";
    case Token::token_log_or:
        return "||";
    case Token::token_bit_not:
        return "~";
    case Token::token_bit_and:
        return "&";
    case Token::token_bit_or:
        return "|";
    case Token::token_bit_xor:
        return "^";
    case Token::token_equal:
        return "==";
    case Token::token_notequal:
        return "!=";
    case Token::token_less:
        return "<";
    case Token::token_lessequal:
        return "<=";
    case Token::token_greater:
        return ">";
    case Token::token_greaterequal:
        return ">=";
    case Token::token_comma:
        return ",";
    case Token::token_semicolon:
        return ";";
    case Token::token_assign:
        return "=";
    case Token::identifier:
        return "IDENTIFIER";
    case Token::numeral:
        return "NUMERAL";
    case Token::literal:
        return "STRING";
    case Token::eof:
        return "END OF FILE";
    default:
        break;
    }

    throw "invalid token";
}

AST::~AST() noexcept {
}

Statement::~Statement() noexcept {
}

Expression::~Expression() noexcept {
}

} /* namespace arabilis */
