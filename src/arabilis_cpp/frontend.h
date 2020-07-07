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

} /* namespace arabilis */

#endif /* FRONTEND_H_ */
