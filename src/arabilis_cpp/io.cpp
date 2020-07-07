// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "io.h"

#include <istream>
#include <ostream>

namespace arabilis {

void Position::advance() noexcept {
    m_column += 1;
}

void Position::advance_to_tabstop() noexcept {
    while (m_column % 8 != 0) {
        advance();
    }
}

void Position::advance_to_newline() noexcept {
    m_column = 0;
    m_line += 1;
}

std::ostream& operator<<(std::ostream& stream, const Position& position) {
    return stream << position.line() << ':' << position.column();
}

/* Read single byte cast to int or -1 on EOF. */
int Reader::read() noexcept {
    m_position.advance();

    const int c = m_stream.get();

    if (m_stream.eof()) {
        return -1;
    }

    if (c == '\t') {
        m_position.advance_to_tabstop();
    }

    if (c == '\n') {
        m_position.advance_to_newline();
    }

    return c;
}

std::ostream& operator<<(std::ostream& stream, const Reader& reader) {
    return stream << reader.filename() << ':' << reader.position() << ':';
}

} /* namespace arabilis */
