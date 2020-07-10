// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#ifndef FRONTEND_H_
#define FRONTEND_H_

#include "ast.h"
#include "io.h"

namespace arabilis {

class Lexer {
public:
    explicit Lexer(Reader& reader) noexcept;

    Lexer(const Lexer&) noexcept = delete;
    Lexer& operator=(const Lexer&) noexcept = delete;

    Lexer(Lexer&&) noexcept = default;
    Lexer& operator=(Lexer&&) noexcept = default;

    ~Lexer() noexcept = default;

    [[nodiscard]] const std::string& filename() const noexcept {
        return m_reader.filename();
    }

    [[nodiscard]] const Position& position() const noexcept {
        return m_position;
    }

    [[nodiscard]] const std::string& data() const noexcept {
        return m_data;
    }

    [[nodiscard]] Token read() noexcept;

private:
    Reader& m_reader;
    Position m_position {};
    std::string m_data {};
    int m_char;
};

class Parser {
public:
    explicit Parser(Lexer& lexer) noexcept;

    Parser(const Parser&) noexcept = delete;
    Parser& operator=(const Parser&) noexcept = delete;

    Parser(Parser&&) noexcept = default;
    Parser& operator=(Parser&&) noexcept = default;

    ~Parser() noexcept = default;

    Program read();

private:
    Lexer& m_lexer;
    Token m_token;

    bool accept(Token);
    void expect(Token);

    int parse_numeral();
    std::string parse_literal();
    std::string parse_identifier();

    Function parse_function();
    GlobalVar parse_globalvar();
    IfStatement parse_statement_if();
    ExpressionStatement parse_statement_expression();
    WhileStatement parse_statement_while();
    ForStatement parse_statement_for();
    VarStatement parse_statement_var();
    LetStatement parse_statement_let();
    ReturnStatement parse_statement_return();
    ContinueStatement parse_statement_continue();
    BreakStatement parse_statement_break();
    std::unique_ptr<Expression> parse_term();
    std::unique_ptr<Expression> parse_factor();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Expression> parse_expression();

};

} /* namespace arabilis */

#endif /* FRONTEND_H_ */
