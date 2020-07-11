// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "backend.h"

#include <iostream>
#include <set>

namespace arabilis {

Visitor::~Visitor() noexcept {
}

class VariableUsage: public Visitor {
public:
    explicit VariableUsage(std::string filename) noexcept:
            m_filename { std::move(filename) } {
    }

    VariableUsage(const VariableUsage&) noexcept = delete;
    VariableUsage& operator=(const VariableUsage&) noexcept = delete;

    VariableUsage(VariableUsage&&) noexcept = default;
    VariableUsage& operator=(VariableUsage&&) noexcept = default;

    ~VariableUsage() noexcept override = default;

    void operator()(const AddressOfExpression*) override;
    void operator()(const BinOpExpression*) override;
    void operator()(const BreakStatement*) override;
    void operator()(const CallExpression*) override;
    void operator()(const ContinueStatement*) override;
    void operator()(const ExpressionStatement*) override;
    void operator()(const ForStatement*) override;
    void operator()(const Function*) override;
    void operator()(const GlobalVar*) override;
    void operator()(const IfStatement*) override;
    void operator()(const LetStatement*) override;
    void operator()(const NumeralExpression*) override;
    void operator()(const Program*) override;
    void operator()(const ReturnStatement*) override;
    void operator()(const StringExpression*) override;
    void operator()(const UnOpExpression*) override;
    void operator()(const VariableExpression*) override;
    void operator()(const VarStatement*) override;
    void operator()(const WhileStatement*) override;

private:
    std::string m_filename;
    bool m_inside_loop = false;
    std::set<std::string> m_global_symbols = {};
    std::set<std::string> m_local_symbols = {};

    /** Create a nested scope. */
    VariableUsage with_block_scope(bool inside_loop) noexcept;

    /** Check that a global variable is unique. */
    void unique_global(const Position&, const std::string&) noexcept;

    /** Check that a local variable is unique in the current context. */
    void unique_local(const Position&, const std::string&) noexcept;

    /** Check that a name refers to either a local or global variable. */
    void check_variable(const Position&, const std::string&) noexcept;
};

void check_variable_usage(Program& program) noexcept {
    VariableUsage variable_usage { program.m_filename };
    program.visit(variable_usage);
}

void VariableUsage::operator()(const AddressOfExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const BinOpExpression* node) {
    node->m_lhs->visit(*this);
    node->m_rhs->visit(*this);
}

void VariableUsage::operator()(const BreakStatement* node) {
    if (m_inside_loop) {
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << node->m_position
        << ": Error: \"break\" outside of FOR or WHILE loop\n";
    std::exit(1);
}

void VariableUsage::operator()(const CallExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
    for (auto& argument : node->m_arguments) {
        argument->visit(*this);
    }
}

void VariableUsage::operator()(const ContinueStatement* node) {
    if (m_inside_loop) {
        return;
    }
    std::cerr
        << m_filename
        << ':'
        << node->m_position
        << ": Error: \"continue\" outside of FOR or WHILE loop\n";
    std::exit(1);
}

void VariableUsage::operator()(const ExpressionStatement* node) {
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const ForStatement* node) {
    node->m_initial->visit(*this);

    VariableUsage inner = with_block_scope(true);
    inner.unique_local(node->m_position, node->m_variable_name);

    node->m_condition->visit(inner);
    node->m_update->visit(inner);

    for (auto& statement : node->m_statements) {
        statement->visit(inner);
    }
}

void VariableUsage::operator()(const Function* node) {
    unique_global(node->m_position, node->m_name);

    m_local_symbols.clear();

    for (auto& argument : node->m_arguments) {
        unique_local(node->m_position, argument);
    }

    for (auto& statement : node->m_statements) {
        statement->visit(*this);
    }
}

void VariableUsage::operator()(const GlobalVar* node) {
    unique_global(node->m_position, node->m_name);
}

void VariableUsage::operator()(const IfStatement* node) {
    node->m_condition->visit(*this);

    VariableUsage inner_then = with_block_scope(m_inside_loop);
    for (auto& statement : node->m_then_statements) {
        statement->visit(inner_then);
    }

    VariableUsage inner_else = with_block_scope(m_inside_loop);
    for (auto& statement : node->m_else_statements) {
        statement->visit(inner_else);
    }
}

void VariableUsage::operator()(const LetStatement* node) {
    check_variable(node->m_position, node->m_variable_name);
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const NumeralExpression* /* node */) {
}

void VariableUsage::operator()(const Program* node) {
    for (auto& globalvar : node->m_globalvars) {
        globalvar.visit(*this);
    }

    for (auto& function : node->m_functions) {
        function.visit(*this);
    }

    const bool has_main = [node]() {
        for (auto& f : node->m_functions) {
            if (f.m_name == "main") {
                return true;
            }
        }
        return false;
    }();

    if (has_main) {
        return;
    }

    std::cerr << m_filename << ": Error: missing \"main\" function\n";
    std::exit(1);
}

void VariableUsage::operator()(const ReturnStatement* node) {
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const StringExpression* /* node */) {
}

void VariableUsage::operator()(const UnOpExpression* node) {
    node->m_rhs->visit(*this);
}

void VariableUsage::operator()(const VariableExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const VarStatement* node) {
    node->m_expression->visit(*this);
    unique_local(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const WhileStatement* node) {
    node->m_condition->visit(*this);

    VariableUsage inner = with_block_scope(true);
    for (auto& statement : node->m_statements) {
        statement->visit(inner);
    }
}

VariableUsage VariableUsage::with_block_scope(bool inside_loop) noexcept {
    VariableUsage variable_usage { m_filename };
    variable_usage.m_global_symbols = m_global_symbols;
    variable_usage.m_local_symbols = m_local_symbols;
    variable_usage.m_inside_loop = inside_loop;
    return variable_usage;
}

void VariableUsage::unique_global(
        const Position& position,
        const std::string& name) noexcept {

    if (m_global_symbols.find(name) == m_global_symbols.end()) {
        m_global_symbols.insert(name);
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: duplicate symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

void VariableUsage::unique_local(
        const Position& position,
        const std::string& name) noexcept {

    if ((m_global_symbols.find(name) == m_global_symbols.end()) &&
            (m_local_symbols.find(name) == m_local_symbols.end())) {

        m_local_symbols.insert(name);
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: duplicate symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

void VariableUsage::check_variable(
        const Position& position,
        const std::string& name) noexcept {

    if ((m_global_symbols.find(name) != m_global_symbols.end()) ||
            (m_local_symbols.find(name) != m_local_symbols.end())) {

        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: unknown symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

} /* namespace arabilis */
