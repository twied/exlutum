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

Parser::Parser(Lexer& lexer) noexcept :
        m_lexer { lexer },
        m_token { Token::eof } {
}

bool Parser::accept(Token token) {
    if (m_token != token) {
        return false;
    }

    m_token = m_lexer.read();
    return true;
}

void Parser::expect(Token token) {
    if (accept(token)) {
        return;
    }

    std::cerr
        << m_lexer.filename()
        << ':'
        << m_lexer.position()
        << ": Error: Unexpected token \""
        << token_to_name(m_token)
        << "\" (expected: \""
        << token_to_name (token)
        << "\")\n";
    std::exit(1);
}

Program Parser::read() {
    Program program { m_lexer.filename() };
    m_token = m_lexer.read();

    while (true) {
        if (m_token == Token::string_function) {
            program.m_functions.push_back(parse_function());
        } else if (m_token == Token::string_var) {
            program.m_globalvars.push_back(parse_globalvar());
        } else {
            break;
        }
    }

    expect(Token::eof);
    return program;
}

Function Parser::parse_function() {
    Function function { m_lexer.position() };

    expect(Token::string_function);

    function.m_name = parse_identifier();

    expect(Token::bracket_round_left);

    if (m_token == Token::identifier) {
        function.m_arguments.push_back(parse_identifier());

        while (m_token == Token::token_comma) {
            expect(Token::token_comma);

            function.m_arguments.push_back(parse_identifier());
        }
    }

    expect(Token::bracket_round_right);

    expect(Token::bracket_curly_left);

    while (m_token != Token::bracket_curly_right) {
        function.m_statements.push_back(parse_statement());
    }

    expect(Token::bracket_curly_right);

    return function;
}

GlobalVar Parser::parse_globalvar() {
    GlobalVar globalvar { m_lexer.position() };

    expect(Token::string_var);

    globalvar.m_name = parse_identifier();

    expect(Token::token_assign);

    const Position position = m_lexer.position();
    if (m_token == Token::literal) {
        globalvar.m_value =
            std::make_unique<StringExpression>(position, parse_literal());
    } else if (m_token == Token::string_true) {
        expect(Token::string_true);
        globalvar.m_value = std::make_unique<NumeralExpression>(position, 1);
    } else if (m_token == Token::string_false) {
        expect(Token::string_false);
        globalvar.m_value = std::make_unique<NumeralExpression>(position, 0);
    } else if (m_token == Token::token_minus) {
        expect(Token::token_minus);
        globalvar.m_value =
            std::make_unique<NumeralExpression>(position, -parse_numeral());
    } else {
        globalvar.m_value =
            std::make_unique<NumeralExpression>(position, parse_numeral());
    }

    expect(Token::token_semicolon);

    return globalvar;
}

ExpressionStatement Parser::parse_statement_expression() {
    ExpressionStatement statement { m_lexer.position() };

    statement.m_expression = parse_expression();

    expect(Token::token_semicolon);

    return statement;
}

IfStatement Parser::parse_statement_if() {
    IfStatement statement { m_lexer.position() };

    expect(Token::string_if);

    expect(Token::bracket_round_left);

    statement.m_condition = parse_expression();

    expect(Token::bracket_round_right);

    expect(Token::bracket_curly_left);

    while (m_token != Token::bracket_curly_right) {
        statement.m_then_statements.push_back(parse_statement());
    }

    expect(Token::bracket_curly_right);

    if (m_token == Token::string_else) {
        expect(Token::string_else);

        expect(Token::bracket_curly_left);

        while (m_token != Token::bracket_curly_right) {
            statement.m_else_statements.push_back(parse_statement());
        }

        expect(Token::bracket_curly_right);
    }

    return statement;
}

WhileStatement Parser::parse_statement_while() {
    WhileStatement statement { m_lexer.position() };

    expect(Token::string_while);

    expect(Token::bracket_round_left);

    statement.m_condition = parse_expression();

    expect(Token::bracket_round_right);

    expect(Token::bracket_curly_left);

    while (m_token != Token::bracket_curly_right) {
        statement.m_statements.push_back(parse_statement());
    }

    expect(Token::bracket_curly_right);

    return statement;
}

ForStatement Parser::parse_statement_for() {
    ForStatement statement { m_lexer.position() };

    expect(Token::string_for);

    expect(Token::bracket_round_left);

    expect(Token::string_var);

    statement.m_variable_name = parse_identifier();

    expect(Token::token_assign);

    statement.m_initial = parse_expression();

    expect(Token::token_semicolon);

    statement.m_condition = parse_expression();

    expect(Token::token_semicolon);

    expect(Token::string_let);

    std::string identifier = parse_identifier();
    if (identifier != statement.m_variable_name) {
        std::cerr
            << m_lexer.filename()
            << ':'
            << m_lexer.position()
            << ": Error: Variable name does not match in \"for\" statement\n";
        std::exit(1);
    }

    expect(Token::token_assign);

    statement.m_update = parse_expression();

    expect(Token::bracket_round_right);

    expect(Token::bracket_curly_left);

    while (m_token != Token::bracket_curly_right) {
        statement.m_statements.push_back(parse_statement());
    }

    expect(Token::bracket_curly_right);

    return statement;
}

VarStatement Parser::parse_statement_var() {
    VarStatement statement { m_lexer.position() };

    expect(Token::string_var);

    statement.m_variable_name = parse_identifier();

    if (m_token == Token::token_assign) {
        expect(Token::token_assign);

        statement.m_expression = parse_expression();
    } else {
        statement.m_expression =
            std::make_unique<NumeralExpression>(m_lexer.position(), 0);
    }

    expect(Token::token_semicolon);

    return statement;
}

LetStatement Parser::parse_statement_let() {
    LetStatement statement { m_lexer.position() };

    expect(Token::string_let);

    statement.m_variable_name = parse_identifier();

    expect(Token::token_assign);

    statement.m_expression = parse_expression();

    expect(Token::token_semicolon);

    return statement;
}

ReturnStatement Parser::parse_statement_return() {
    ReturnStatement statement { m_lexer.position() };

    expect(Token::string_return);

    if (m_token != Token::token_semicolon) {
        statement.m_expression = parse_expression();
    } else {
        statement.m_expression =
            std::make_unique<NumeralExpression>(m_lexer.position(), 0);
    }

    expect(Token::token_semicolon);

    return statement;
}

ContinueStatement Parser::parse_statement_continue() {
    ContinueStatement statement { m_lexer.position() };

    expect(Token::string_continue);

    expect(Token::token_semicolon);

    return statement;
}

BreakStatement Parser::parse_statement_break() {
    BreakStatement statement { m_lexer.position() };

    expect(Token::string_break);

    expect(Token::token_semicolon);

    return statement;
}

std::unique_ptr<Statement> Parser::parse_statement() {
    switch (m_token) {
    case Token::string_if:
        return std::make_unique<IfStatement>(parse_statement_if());
    case Token::string_while:
        return std::make_unique<WhileStatement>(parse_statement_while());
    case Token::string_for:
        return std::make_unique<ForStatement>(parse_statement_for());
    case Token::string_var:
        return std::make_unique<VarStatement>(parse_statement_var());
    case Token::string_let:
        return std::make_unique<LetStatement>(parse_statement_let());
    case Token::string_return:
        return std::make_unique<ReturnStatement>(parse_statement_return());
    case Token::string_continue:
        return std::make_unique<ContinueStatement>(parse_statement_continue());
    case Token::string_break:
        return std::make_unique<BreakStatement>(parse_statement_break());
    default:
        break;
    }

    return std::make_unique<ExpressionStatement>(parse_statement_expression());
}

std::unique_ptr<Expression> Parser::parse_expression() {
    Position position = m_lexer.position();

    std::unique_ptr<Expression> lhs = parse_term();
    if (m_token == Token::token_plus || m_token == Token::token_minus ||
        m_token == Token::token_multiply || m_token == Token::token_divide ||
        m_token == Token::token_modulo || m_token == Token::token_log_and ||
        m_token == Token::token_log_or || m_token == Token::token_bit_and ||
        m_token == Token::token_bit_or || m_token == Token::token_bit_xor ||
        m_token == Token::token_equal || m_token == Token::token_notequal ||
        m_token == Token::token_less || m_token == Token::token_lessequal ||
        m_token == Token::token_greater ||
        m_token == Token::token_greaterequal) {
        const Token token = m_token;
        expect(token);

        return std::make_unique<BinOpExpression>(
            position, token, std::move(lhs), parse_term());
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::parse_term() {
    Position position = m_lexer.position();

    if (m_token == Token::token_plus || m_token == Token::token_minus ||
        m_token == Token::token_log_not || m_token == Token::token_bit_not) {
        const Token token = m_token;
        expect(token);

        return std::make_unique<UnOpExpression>(
            position, token, parse_factor());
    }

    return parse_factor();
}

std::unique_ptr<Expression> Parser::parse_factor() {
    const Position position = m_lexer.position();

    if (m_token == Token::identifier) {
        std::string identifier = parse_identifier();

        if (m_token != Token::bracket_round_left) {
            return std::make_unique<VariableExpression>(position, identifier);
        }

        auto expression =
            std::make_unique<CallExpression>(position, identifier);

        expect(Token::bracket_round_left);

        if (m_token != Token::bracket_round_right) {
            expression->m_arguments.push_back(parse_expression());

            while (m_token == Token::token_comma) {
                expect(Token::token_comma);

                expression->m_arguments.push_back(parse_expression());
            }
        }

        expect(Token::bracket_round_right);

        return expression;
    }

    if (m_token == Token::token_bit_and) {
        expect(m_token);

        return std::make_unique<AddressOfExpression>(
            position, parse_identifier());
    }

    if (m_token == Token::string_true) {
        expect(m_token);

        return std::make_unique<NumeralExpression>(position, 1);
    }

    if (m_token == Token::string_false) {
        expect(m_token);

        return std::make_unique<NumeralExpression>(position, 0);
    }

    if (m_token == Token::literal) {
        return std::make_unique<StringExpression>(position, parse_literal());
    }

    if (m_token == Token::bracket_round_left) {
        expect(Token::bracket_round_left);

        std::unique_ptr<Expression> expression = parse_expression();

        expect(Token::bracket_round_right);

        return expression;
    }

    return std::make_unique<NumeralExpression>(position, parse_numeral());
}

std::string Parser::parse_identifier() {
    std::string identifier = m_lexer.data();

    expect(Token::identifier);

    return identifier;
}

int Parser::parse_numeral() {
    int numeral = 0;

    for (const char c : m_lexer.data()) {
        numeral = numeral * 10 + c - '0';
    }

    expect(Token::numeral);

    return numeral;
}

std::string Parser::parse_literal() {
    std::string literal = m_lexer.data();

    expect(Token::literal);

    return literal;
}

} /* namespace arabilis */
