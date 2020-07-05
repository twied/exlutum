// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#ifndef IO_H_
#define IO_H_

#include <iosfwd>
#include <utility>
#include <string>

namespace arabilis {

class Position {
public:
    Position() noexcept : m_line { 1 }, m_column { 0 } {
    }

    Position(int line, int column) noexcept:
            m_line { line },
            m_column { column } {
    }

    Position(const Position&) noexcept = default;
    Position& operator=(const Position&) noexcept = default;

    Position(Position&&) noexcept = default;
    Position& operator=(Position&&) noexcept = default;

    ~Position() noexcept = default;

    [[nodiscard]] int line() const noexcept {
        return m_line;
    }

    [[nodiscard]] int column() const noexcept {
        return m_column;
    }

    void advance() noexcept;

    void advance_to_tabstop() noexcept;

    void advance_to_newline() noexcept;

    friend std::ostream& operator<<(std::ostream&, const Position&);

private:
    int m_line;
    int m_column;
};

class Reader {
public:
    Reader(std::string filename, std::istream& stream) noexcept:
            m_filename { std::move(filename) },
            m_stream { stream } {
    }

    Reader(const Reader&) noexcept = delete;
    Reader& operator=(const Reader&) noexcept = delete;

    Reader(Reader&&) noexcept = default;
    Reader& operator=(Reader&&) noexcept = default;

    ~Reader() noexcept = default;

    [[nodiscard]] const std::string& filename() const noexcept {
        return m_filename;
    }

    [[nodiscard]] const Position& position() const noexcept {
        return m_position;
    }

    /* Read single byte cast to int or -1 on EOF. */
    int read() noexcept;

private:
    std::string m_filename;
    std::istream& m_stream;
    Position m_position {};
};

class Writer {
public:
    explicit Writer(std::ostream& stream) noexcept:
        m_stream { stream } {
    }

    Writer(const Writer&) noexcept = delete;
    Writer& operator=(const Writer&) noexcept = delete;

    Writer(Writer&&) noexcept = default;
    Writer& operator=(Writer&&) noexcept = default;

    ~Writer() noexcept = default;

    template <typename T>
    friend Writer& operator<<(Writer& writer, const T& value) {
        writer.m_stream << value;
        return writer;
    }

private:
    std::ostream& m_stream;
};

} /* namespace arabilis */

#endif /* IO_H_ */
