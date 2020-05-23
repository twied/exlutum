// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

// poor man's static_assert()
char must_be_compiled_for_32bit_environment[sizeof(void*) == 4 ? 1: -1];

// we want to assume contiguous memory when allocating new memory.
#define HEAP_SIZE (10 * 1024 * 1024)
static char HEAP[HEAP_SIZE];

#define HEAD 0x08048000

struct label {
    unsigned long address;
    char name[16]; /* not null-terminated! */
};

int syscall_x86_32(int eax, int ebx, int ecx, int edx) {
    asm volatile ("int $0x80" : "+a" (eax) : "b" (ebx), "c" (ecx), "d" (edx));
    return eax;
}

static void exit(int status) {
    syscall_x86_32(1, status, 0, 0);
}

static int getchar() {
    char buffer;
    int result;

    result = syscall_x86_32(3, 0, (int) &buffer, 1);
    if (result == 1) {
        return buffer;
    } else {
        return -1;
    }
}

static int putchar(int c) {
    char buffer;
    int result;

    buffer = c;
    result = syscall_x86_32(4, 1, (int) &buffer, 1);
    return result;
}

static int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static int is_hexdigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static int is_id_start(char c) {
    return c >= 'a' && c <= 'z';
}

static int is_id_cont(char c) {
    return is_id_start(c) || (c >= '0' && c <= '9') || c == '_';
}

static char* read_input(char* input_begin) {
    char* it;
    int c;

    it = input_begin;
    while (1) {
        if (it >= HEAP + HEAP_SIZE) {
            exit(1);
        }

        c = getchar();
        if (c < 0) {
            break;
        }

        if (c == '#') {
            while(1) {
                c = getchar();
                if (c < 0 || c == '\r' || c == '\n') {
                    break;
                }
            }

            c = ' ';
        }

        *it = c;
        ++it;
    }

    return it;
}

static int read_byte(char* input_begin, char* input_end, char* it) {
    if (it >= input_begin && it < input_end) {
        return *it;
    }

    return -1;
}

static struct label* read_labels(
        char* input_begin,
        char* input_end,
        struct label* labels_begin) {

    struct label* labels_end;
    unsigned location;
    char* it;
    int i;
    char* delete;

    labels_end = labels_begin;
    location = 0;
    it = input_begin;

    while(1) {
        if (it >= input_end) {
            break;
        }

        i = read_byte(input_begin, input_end, it);

        // skip spaces
        if (is_space(i)) {
            it += 1;
            continue;
        }

        // hex digits take up 1 byte in output
        if (is_hexdigit(i)) {
            location += 1;

            // no need to check validity of second hex digit, this
            // will be caught in `hex2bin`
            it += 2;
            continue;
        }

        // labels take up 4 bytes in output, but may be of variable
        // length in the input.
        if (is_id_start(i)) {
            location += 4;

            while(1) {
                i = read_byte(input_begin, input_end, it);
                if (!is_id_cont(i)) {
                    break;
                }

                it += 1;
            }
            continue;
        }

        // definitions take up 0 bytes in output, but may be of
        // variable length. blank them out after processing.
        if (i == '.') {
            delete = it;

            if ((char*)(labels_end + 1) > (HEAP + HEAP_SIZE)) {
                exit(1);
            }

            for (i = 0; i < 16; ++i) {
                labels_end->name[i] = '\0';
            }

            labels_end->address = HEAD + location;

            labels_end->name[0] = read_byte(input_begin, input_end, ++it);
            if (!is_id_start(labels_end->name[0])) {
                exit(1);
            }

            i = 1;
            while (1) {
                if (i >= 16) {
                    break;
                }

                labels_end->name[i] = read_byte(input_begin, input_end, ++it);
                if (labels_end->name[i] == ':') {
                    labels_end->name[i] = '\0';
                }

                if (!is_id_cont(labels_end->name[i])) {
                    break;
                }

                i = i + 1;
            }

            if (read_byte(input_begin, input_end, it) != ':') {
                exit(1);
            }

            ++labels_end;
            ++it;

            while(delete < it) {
                *delete = ' ';
                ++delete;
            }

            continue;
        }

        exit(1);
    }

    if ((char*)(labels_end + 1) > (HEAP + HEAP_SIZE)) {
        exit(1);
    }

    labels_end->address = location;
    labels_end->name[0] = 's';
    labels_end->name[1] = 'i';
    labels_end->name[2] = 'z';
    labels_end->name[3] = 'e';
    labels_end->name[4] = '\0';

    return labels_end + 1;
}

static int correct_label(struct label* label, char* name) {
    int i = 0;

    while(1) {
        if (i >= 16) {
            break;
        }

        if (label->name[i] == '\0') {
            break;
        }

        if (label->name[i] != name[i]) {
            return 0;
        }

        i += 1;
    }

    return i;
}

static void putnybble(int value) {
    if (value < 10) {
        putchar('0' + value);
    } else {
        putchar('A' + value - 10);
    }
}

static void putbyte(int value) {
    putnybble(0x0f & (value >> 4));
    putnybble(0x0f & (value >> 0));
}

static int write_label(struct label* labels_begin, struct label* labels_end, char* name) {
    struct label* label = labels_begin;
    int length;

    while (1) {
        if (label >= labels_end) {
            exit(1);
        }

        length = correct_label(label, name);
        if (length == 0) {
            label += 1;
            continue;
        }

        putbyte(0xff & (label->address >> 0));
        putbyte(0xff & (label->address >> 8));
        putbyte(0xff & (label->address >> 16));
        putbyte(0xff & (label->address >> 24));
        break;
    }

    return length;
}

static void write_output(
        char* input_begin,
        char* input_end,
        struct label* labels_begin,
        struct label* labels_end) {

    char* it = input_begin;

    while (1) {
        if (it >= input_end) {
            break;
        }

        if (is_id_start(*it)) {
            it += write_label(labels_begin, labels_end, it);
            continue;
        }

        putchar(*it);
        it += 1;
    }
}

int main() {
    char* input_begin;
    char* input_end;
    struct label* labels_begin;
    struct label* labels_end;

    input_begin = HEAP;
    input_end = read_input(input_begin);

    // aligned to next long
    labels_begin = (struct label*) (
        ~0x03 & ((unsigned long) input_end + sizeof(long) - 1));
    labels_end = read_labels(input_begin, input_end, labels_begin);

    write_output(input_begin, input_end, labels_begin, labels_end);
    return 0;
}
