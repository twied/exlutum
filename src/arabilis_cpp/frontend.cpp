// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "frontend.h"

#include <iostream>

namespace arabilis {

static constexpr bool is_whitespace(int c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static constexpr bool is_alpha(int c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static constexpr bool is_numeral(int c) {
    return c >= '0' && c <= '9';
}

static constexpr bool is_id_begin(int c) {
    return is_alpha(c) || c == '_';
}

static constexpr bool is_id_trail(int c) {
    return is_id_begin(c) || is_numeral(c);
}

static constexpr bool is_hex(int c) {
    return is_numeral(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static constexpr int valueof_hex(int c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    return c - 'A' + 10;
}

Lexer::Lexer(Reader& reader) noexcept :
        m_reader { reader },
        m_char { reader.read() } {
}

Token Lexer::read() noexcept {
    while (is_whitespace(m_char) || m_char == '#') {
        /* discard whitespace */
        while (is_whitespace(m_char)) {
            m_position = m_reader.position();
            m_char = m_reader.read();
        }

        /* discard comments */
        if (m_char == '#') {
            m_position = m_reader.position();
            while (m_char != -1 && m_char != '\n') {
                m_position = m_reader.position();
                m_char = m_reader.read();
            }
        }
    }

    /* end of file */
    if (m_char == -1) {
        return Token::eof;
    }

    /* identifier or keyword */
    if (is_id_begin(m_char)) {
        m_data.clear();
        while (is_id_trail(m_char)) {
            m_data += static_cast<char>(m_char);
            m_char = m_reader.read();
        }

        if (m_data == "break") {
            return Token::string_break;
        }
        if (m_data == "continue") {
            return Token::string_continue;
        }
        if (m_data == "else") {
            return Token::string_else;
        }
        if (m_data == "false") {
            return Token::string_false;
        }
        if (m_data == "for") {
            return Token::string_for;
        }
        if (m_data == "function") {
            return Token::string_function;
        }
        if (m_data == "if") {
            return Token::string_if;
        }
        if (m_data == "let") {
            return Token::string_let;
        }
        if (m_data == "return") {
            return Token::string_return;
        }
        if (m_data == "true") {
            return Token::string_true;
        }
        if (m_data == "var") {
            return Token::string_var;
        }
        if (m_data == "while") {
            return Token::string_while;
        }

        return Token::identifier;
    }

    /* numeral */
    if (is_numeral(m_char)) {
        m_data.clear();
        while (is_numeral(m_char)) {
            m_data += static_cast<char>(m_char);
            m_char = m_reader.read();
        }

        return Token::numeral;
    }

    /* string literal */
    if (m_char == '"') {
        m_data.clear();

        m_char = m_reader.read();
        while (m_char != '\"') {
            if (m_char == -1) {
                std::cerr
                    << m_reader
                    << ": Error: Missing terminating \" character\n";
                std::exit(1);
            } else if (m_char == '\\') {
                m_char = m_reader.read();

                if (m_char == '\\') {
                    m_data += '\\';
                } else if (m_char == '"') {
                    m_data += '"';
                } else if (m_char == 'n') {
                    m_data += '\n';
                } else if (m_char == 'r') {
                    m_data += '\r';
                } else if (m_char == 't') {
                    m_data += '\t';
                } else if (m_char == 'x') {
                    int char1 = m_reader.read();
                    int char2 = m_reader.read();
                    if (!is_hex(char1) || !is_hex(char2)) {
                        std::cerr
                            << m_reader
                            << ": Error: Invalid escape sequence\n";
                        std::exit(1);
                    }
                    m_data += static_cast<char>(
                        16 * valueof_hex(char1) + valueof_hex(char2));
                } else {
                    std::cerr
                        << m_reader
                        << ": Error: Unknown escape sequence\n";
                    std::exit(1);
                }
            } else {
                m_data += static_cast<char>(m_char);
            }

            m_char = m_reader.read();
        }

        m_char = m_reader.read();
        return Token::literal;
    }

    const int last_char = m_char;
    m_char = m_reader.read();

    switch (last_char) {
    case '{':
        return Token::bracket_curly_left;
    case '}':
        return Token::bracket_curly_right;
    case '(':
        return Token::bracket_round_left;
    case ')':
        return Token::bracket_round_right;
    case ',':
        return Token::token_comma;
    case ';':
        return Token::token_semicolon;
    case '+':
        return Token::token_plus;
    case '-':
        return Token::token_minus;
    case '*':
        return Token::token_multiply;
    case '/':
        return Token::token_divide;
    case '%':
        return Token::token_modulo;
    case '~':
        return Token::token_bit_not;
    case '^':
        return Token::token_bit_xor;
    case '&':
        if (m_char == '&') {
            m_char = m_reader.read();
            return Token::token_log_and;
        }
        return Token::token_bit_and;
    case '|':
        if (m_char == '|') {
            m_char = m_reader.read();
            return Token::token_log_or;
        }
        return Token::token_bit_or;
    case '=':
        if (m_char == '=') {
            m_char = m_reader.read();
            return Token::token_equal;
        }
        return Token::token_assign;
    case '!':
        if (m_char == '=') {
            m_char = m_reader.read();
            return Token::token_notequal;
        }
        return Token::token_log_not;
    case '<':
        if (m_char == '=') {
            m_char = m_reader.read();
            return Token::token_lessequal;
        }
        return Token::token_less;
    case '>':
        if (m_char == '=') {
            m_char = m_reader.read();
            return Token::token_greaterequal;
        }
        return Token::token_greater;
    default:
        break;
    }

    std::cerr << m_reader << ": Error: Unexpected character\n";
    std::exit(1);
}

} /* namespace arabilis */
