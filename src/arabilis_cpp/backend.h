// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#ifndef BACKEND_H_
#define BACKEND_H_

#include "ast.h"

namespace arabilis {

class Visitor {
public:
    explicit Visitor() noexcept = default;

    Visitor(const Visitor&) noexcept = default;
    Visitor& operator=(const Visitor&) noexcept = default;

    Visitor(Visitor&&) noexcept = default;
    Visitor& operator=(Visitor&&) noexcept = default;

    virtual ~Visitor() noexcept = 0;

    virtual void operator()(const AddressOfExpression*) = 0;
    virtual void operator()(const BinOpExpression*) = 0;
    virtual void operator()(const BreakStatement*) = 0;
    virtual void operator()(const CallExpression*) = 0;
    virtual void operator()(const ContinueStatement*) = 0;
    virtual void operator()(const ExpressionStatement*) = 0;
    virtual void operator()(const ForStatement*) = 0;
    virtual void operator()(const Function*) = 0;
    virtual void operator()(const GlobalVar*) = 0;
    virtual void operator()(const IfStatement*) = 0;
    virtual void operator()(const LetStatement*) = 0;
    virtual void operator()(const NumeralExpression*) = 0;
    virtual void operator()(const Program*) = 0;
    virtual void operator()(const ReturnStatement*) = 0;
    virtual void operator()(const StringExpression*) = 0;
    virtual void operator()(const UnOpExpression*) = 0;
    virtual void operator()(const VariableExpression*) = 0;
    virtual void operator()(const VarStatement*) = 0;
    virtual void operator()(const WhileStatement*) = 0;
};

void check_variable_usage(Program&) noexcept;

} /* namespace arabilis */

#endif /* BACKEND_H_ */
