// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include <cstdio>
#include <map>
#include <string>
#include <vector>

constexpr static uint32_t HEAD = 0x08048000;

static bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static bool is_hexdigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static bool is_id_start(char c) {
    return c >= 'a' && c <= 'z';
}

static bool is_id_cont(char c) {
    return is_id_start(c) || (c >= '0' && c <= '9') || (c == '_');
}

static std::vector<char> read_input() {
    std::vector<char> input {};
    for (;;) {
        const int value = getchar();

        if (value < 0) {
            break;
        }

        // remove comments from input
        if (value == '#') {
            input.push_back(' ');
            for (;;) {
                const int in_comment = getchar();
                if (in_comment < 0) {
                    break;
                }
                if (in_comment == '\r' || in_comment == '\n') {
                    break;
                }
            }
            continue;
        }

        input.push_back(value);
    }

    return input;
}

static int read_byte(const std::vector<char>& input, size_t index) {
    if (index < input.size()) {
        return input[index];
    }

    return -1;
}

static void write_byte(std::vector<char>& input, size_t index, char c) {
    if (index < input.size()) {
        input[index] = c;
    }
}

static std::map<std::string, uint32_t> read_labels(std::vector<char>& input) {
    std::map<std::string, uint32_t> labels {};
    uint32_t location {};

    for(size_t i = 0; i < input.size(); /* nothing */) {
        const char c = read_byte(input, i);

        // skip spaces
        if (is_space(c)) {
            i += 1;
            continue;
        }

        // hex digits take up 1 byte in output
        if (is_hexdigit(c)) {
            location += 1;

            // no need to check validity of second hex digit, this
            // will be caught in `hex2bin`
            i += 2;
            continue;
        }

        // labels take up 4 bytes in output, but may be of variable
        // length in the input.
        if (is_id_start(c)) {
            location += 4;

            while(is_id_cont(read_byte(input, i))) {
                i += 1;
            }
            continue;
        }

        // definitions take up 0 bytes in output, but may be of
        // variable length. blank them out after processing.
        if (c == '.') {
            size_t begin = i;

            char label_c = read_byte(input, ++i);
            if (!is_id_start(label_c)) {
                fprintf(
                    stderr,
                    "Error: Label starts with invalid character '%c' (%i)\n",
                    label_c,
                    label_c);
                std::exit(1);
            }
            
            std::string label {label_c};
            
            while(is_id_cont(label_c = read_byte(input, ++i))) {
                label += label_c;
            }
            
            if (label.size() > 16) {
                fprintf(
                    stderr,
                    "Error: Label \"%s\" too long\n",
                    label.c_str());
                std::exit(1);
            }
            
            labels[label] = HEAD + location;
            
            if (label_c != ':') {
                fprintf(
                    stderr,
                    "Error: Expected ':' after definition of label \"%s\"\n",
                    label.c_str());
                std::exit(1);
            }
            
            i += 1;
            
            // blank out definition
            for(auto del = begin; del != i; ++del) {
                write_byte(input, del, ' ');
            }
            
            continue;
        }

        fprintf(stderr, "Error: Unrecognized character '%c' (%i)\n", c, c);
        std::exit(1);
    }

    if (labels.find("size") == labels.end()) {
        labels["size"] = location;
    }

    return labels;
}

static void write_hex(uint32_t value) {
    const char digits[] = "0123456789ABCDEF";

    putchar(digits[0x0f & (value >> 4)]);
    putchar(digits[0x0f & (value >> 0)]);

    putchar(digits[0x0f & (value >> 12)]);
    putchar(digits[0x0f & (value >> 8)]);

    putchar(digits[0x0f & (value >> 20)]);
    putchar(digits[0x0f & (value >> 16)]);

    putchar(digits[0x0f & (value >> 28)]);
    putchar(digits[0x0f & (value >> 24)]);
}

static void write_output(
        const std::vector<char>& input,
        const std::map<std::string, uint32_t>& labels) {

    char last_char_was_space { true };
    for (auto it = input.begin(); it != input.end(); ) {
        if (is_id_start(*it)) {
            std::string label {*it};

            while (is_id_cont(*++it)) {
                label += *it;
            }

            if(labels.find(label) == labels.end()) {
                fprintf(
                    stderr,
                    "Error: Undefined label \"%s\"\n",
                    label.c_str());
                std::exit(1);
            }

            write_hex(labels.at(label));
            last_char_was_space = false;
            continue;
        }

        if (!last_char_was_space || *it != ' ') {
            putchar(*it);
        }

        last_char_was_space = is_space(*it);
        ++it;
    }
}

int main(void) {
    auto input = read_input();
    auto labels = read_labels(input);

    write_output(input, labels);
    return 0;
}
