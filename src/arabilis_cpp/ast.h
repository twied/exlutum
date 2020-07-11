// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#ifndef AST_H_
#define AST_H_

#include "io.h"

#include <memory>
#include <string>
#include <vector>

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

class Visitor;

struct AST {
    explicit AST(const Position& position) noexcept : m_position { position } {
    }

    AST(const AST&) noexcept = default;
    AST& operator=(const AST&) noexcept = default;

    AST(AST&&) noexcept = default;
    AST& operator=(AST&&) noexcept = default;

    virtual ~AST() noexcept = 0;

    virtual void visit(Visitor&) const noexcept = 0;

    Position m_position;
};

struct Statement: public AST {
    explicit Statement(const Position& position) noexcept : AST { position } {
    }

    Statement(const Statement&) noexcept = delete;
    Statement& operator=(const Statement&) noexcept = delete;

    Statement(Statement&&) noexcept = default;
    Statement& operator=(Statement&&) noexcept = default;

    ~Statement() noexcept override = 0;

    void visit(Visitor&) const noexcept override = 0;
};

struct Expression: public AST {
    explicit Expression(const Position& position) noexcept : AST { position } {
    }

    Expression(const Expression&) noexcept = delete;
    Expression& operator=(const Expression&) noexcept = delete;

    Expression(Expression&&) noexcept = delete;
    Expression& operator=(Expression&&) noexcept = delete;

    ~Expression() noexcept override = 0;

    void visit(Visitor&) const noexcept override = 0;
};

struct GlobalVar: public AST {
    explicit GlobalVar(const Position& position) noexcept : AST { position } {
    }

    GlobalVar(const GlobalVar&) noexcept = delete;
    GlobalVar& operator=(const GlobalVar&) noexcept = delete;

    GlobalVar(GlobalVar&&) noexcept = default;
    GlobalVar& operator=(GlobalVar&&) noexcept = default;

    ~GlobalVar() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_name;
    std::unique_ptr<Expression> m_value;
};

struct Function: public AST {
    explicit Function(const Position& position) noexcept : AST { position } {
    }

    Function(const Function&) noexcept = delete;
    Function& operator=(const Function&) noexcept = delete;

    Function(Function&&) noexcept = default;
    Function& operator=(Function&&) noexcept = default;

    ~Function() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_name;
    std::vector<std::string> m_arguments;
    std::vector<std::unique_ptr<Statement>> m_statements;
};

struct Program: public AST {
    explicit Program(std::string filename) noexcept :
            AST { Position {} },
            m_filename { std::move(filename) } {
    }

    Program(const Program&) noexcept = delete;
    Program& operator=(const Program&) noexcept = delete;

    Program(Program&&) noexcept = default;
    Program& operator=(Program&&) noexcept = default;

    ~Program() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_filename;
    std::vector<GlobalVar> m_globalvars;
    std::vector<Function> m_functions;
};

struct BreakStatement: public Statement {
    explicit BreakStatement(const Position& position) noexcept :
            Statement { position } {
    }

    BreakStatement(const BreakStatement&) noexcept = delete;
    BreakStatement& operator=(const BreakStatement&) noexcept = delete;

    BreakStatement(BreakStatement&&) noexcept = default;
    BreakStatement& operator=(BreakStatement&&) noexcept = default;

    ~BreakStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;
};

struct ContinueStatement: public Statement {
    explicit ContinueStatement(const Position& position) noexcept :
            Statement { position } {
    }

    ContinueStatement(const ContinueStatement&) noexcept = delete;
    ContinueStatement& operator=(const ContinueStatement&) noexcept = delete;

    ContinueStatement(ContinueStatement&&) noexcept = default;
    ContinueStatement& operator=(ContinueStatement&&) noexcept = default;

    ~ContinueStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;
};

struct ExpressionStatement: public Statement {
    explicit ExpressionStatement(const Position& position) noexcept :
            Statement { position } {
    }

    ExpressionStatement(const ExpressionStatement&) noexcept = delete;
    ExpressionStatement& operator=(const ExpressionStatement&) noexcept =
        delete;

    ExpressionStatement(ExpressionStatement&&) noexcept = default;
    ExpressionStatement& operator=(ExpressionStatement&&) noexcept = default;

    ~ExpressionStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::unique_ptr<Expression> m_expression;
};

struct ForStatement: public Statement {
    explicit ForStatement(const Position& position) noexcept :
            Statement { position } {
    }

    ForStatement(const ForStatement&) noexcept = delete;
    ForStatement& operator=(const ForStatement&) noexcept = delete;

    ForStatement(ForStatement&&) noexcept = default;
    ForStatement& operator=(ForStatement&&) noexcept = default;

    ~ForStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
    std::unique_ptr<Expression> m_initial;
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Expression> m_update;
    std::vector<std::unique_ptr<Statement>> m_statements;
};

struct IfStatement: public Statement {
    explicit IfStatement(const Position& position) noexcept :
            Statement { position } {
    }

    IfStatement(const IfStatement&) noexcept = delete;
    IfStatement& operator=(const IfStatement&) noexcept = delete;

    IfStatement(IfStatement&&) noexcept = default;
    IfStatement& operator=(IfStatement&&) noexcept = default;

    ~IfStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::unique_ptr<Expression> m_condition;
    std::vector<std::unique_ptr<Statement>> m_then_statements;
    std::vector<std::unique_ptr<Statement>> m_else_statements;
};

struct LetStatement: public Statement {
    explicit LetStatement(const Position& position) noexcept :
            Statement { position } {
    }

    LetStatement(const LetStatement&) noexcept = delete;
    LetStatement& operator=(const LetStatement&) noexcept = delete;

    LetStatement(LetStatement&&) noexcept = default;
    LetStatement& operator=(LetStatement&&) noexcept = default;

    ~LetStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
    std::unique_ptr<Expression> m_expression;
};

struct ReturnStatement: public Statement {
    explicit ReturnStatement(const Position& position) noexcept :
            Statement { position } {
    }

    ReturnStatement(const ReturnStatement&) noexcept = delete;
    ReturnStatement& operator=(const ReturnStatement&) noexcept = delete;

    ReturnStatement(ReturnStatement&&) noexcept = default;
    ReturnStatement& operator=(ReturnStatement&&) noexcept = default;

    ~ReturnStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::unique_ptr<Expression> m_expression;
};

struct VarStatement: public Statement {
    explicit VarStatement(const Position& position) noexcept :
            Statement { position } {
    }

    VarStatement(const VarStatement&) noexcept = delete;
    VarStatement& operator=(const VarStatement&) noexcept = delete;

    VarStatement(VarStatement&&) noexcept = default;
    VarStatement& operator=(VarStatement&&) noexcept = default;

    ~VarStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
    std::unique_ptr<Expression> m_expression;
};

struct WhileStatement: public Statement {
    explicit WhileStatement(const Position& position) noexcept :
            Statement { position } {
    }

    WhileStatement(const WhileStatement&) noexcept = delete;
    WhileStatement& operator=(const WhileStatement&) noexcept = delete;

    WhileStatement(WhileStatement&&) noexcept = default;
    WhileStatement& operator=(WhileStatement&&) noexcept = default;

    ~WhileStatement() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::unique_ptr<Expression> m_condition;
    std::vector<std::unique_ptr<Statement>> m_statements;
};

struct AddressOfExpression: public Expression {
    explicit AddressOfExpression(
            const Position& position,
            std::string variable_name) noexcept :
                    Expression { position },
                    m_variable_name { std::move(variable_name) } {
    }

    AddressOfExpression(const AddressOfExpression&) noexcept = delete;
    AddressOfExpression& operator=(const AddressOfExpression&) noexcept =
        delete;

    AddressOfExpression(AddressOfExpression&&) noexcept = default;
    AddressOfExpression& operator=(AddressOfExpression&&) noexcept = default;

    ~AddressOfExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
};

struct BinOpExpression: public Expression {
    explicit BinOpExpression(
            const Position& position,
            Token token,
            std::unique_ptr<Expression> lhs,
            std::unique_ptr<Expression> rhs) noexcept :
                    Expression { position },
                    m_token { token },
                    m_lhs { std::move(lhs) },
                    m_rhs { std::move(rhs) } {
    }

    BinOpExpression(const BinOpExpression&) noexcept = delete;
    BinOpExpression& operator=(const BinOpExpression&) noexcept = delete;

    BinOpExpression(BinOpExpression&&) noexcept = default;
    BinOpExpression& operator=(BinOpExpression&&) noexcept = default;

    ~BinOpExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    Token m_token;
    std::unique_ptr<Expression> m_lhs;
    std::unique_ptr<Expression> m_rhs;
};

struct CallExpression: public Expression {
    explicit CallExpression(
            const Position& position,
            std::string variable_name) noexcept :
                    Expression { position },
                    m_variable_name { std::move(variable_name) },
                    m_arguments {} {
    }

    CallExpression(const CallExpression&) noexcept = delete;
    CallExpression& operator=(const CallExpression&) noexcept = delete;

    CallExpression(CallExpression&&) noexcept = default;
    CallExpression& operator=(CallExpression&&) noexcept = default;

    ~CallExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

struct NumeralExpression: public Expression {
    explicit NumeralExpression(const Position& position, int value) noexcept :
            Expression { position },
            m_value { value } {
    }

    NumeralExpression(const NumeralExpression&) noexcept = delete;
    NumeralExpression& operator=(const NumeralExpression&) noexcept = delete;

    NumeralExpression(NumeralExpression&&) noexcept = default;
    NumeralExpression& operator=(NumeralExpression&&) noexcept = default;

    ~NumeralExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    int m_value;
};

struct StringExpression: public Expression {
    explicit StringExpression(
            const Position& position,
            std::string value) noexcept :
                    Expression { position },
                    m_value { value } {
    }

    StringExpression(const StringExpression&) noexcept = delete;
    StringExpression& operator=(const StringExpression&) noexcept = delete;

    StringExpression(StringExpression&&) noexcept = default;
    StringExpression& operator=(StringExpression&&) noexcept = default;

    ~StringExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_value;
};

struct UnOpExpression: public Expression {
    explicit UnOpExpression(
            const Position& position,
            Token token,
            std::unique_ptr<Expression> rhs) noexcept :
                    Expression { position },
                    m_token { token },
                    m_rhs { std::move(rhs) } {
    }

    UnOpExpression(const UnOpExpression&) noexcept = delete;
    UnOpExpression& operator=(const UnOpExpression&) noexcept = delete;

    UnOpExpression(UnOpExpression&&) noexcept = default;
    UnOpExpression& operator=(UnOpExpression&&) noexcept = default;

    ~UnOpExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    Token m_token;
    std::unique_ptr<Expression> m_rhs;
};

struct VariableExpression: public Expression {
    explicit VariableExpression(
            const Position& position,
            std::string variable_name) noexcept :
                    Expression { position },
                    m_variable_name { variable_name } {
    }

    VariableExpression(const VariableExpression&) noexcept = delete;
    VariableExpression& operator=(const VariableExpression&) noexcept = delete;

    VariableExpression(VariableExpression&&) noexcept = default;
    VariableExpression& operator=(VariableExpression&&) noexcept = default;

    ~VariableExpression() noexcept override = default;

    void visit(Visitor&) const noexcept override;

    std::string m_variable_name;
};

} /* namespace arabilis */

#endif /* AST_H_ */
