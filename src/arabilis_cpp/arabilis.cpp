// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "io.h"

#include <iostream>
#include <fstream>
#include <string>

enum class mode {
    only_io,
    default_mode
};

static void usage(std::ostream& stream) {
    stream
        << "Usage: arabilis [options] file\n"
        << "Options:\n"
        << "--help                  Display this information.\n"
        << "-o, --out-file <file>   Place the output into <file>. " \
            "Defaults to stdout.\n";
}

static int mode_only_io(arabilis::Reader& reader, arabilis::Writer& writer) {
    const std::string& filename = reader.filename();

    for (;;) {
        const arabilis::Position position = reader.position();

        const int c = reader.read();
        if (c < 0) {
            break;
        }

        const char v = (c >= 32 && c < 127) ? c : '?';

        writer
            << filename
            << ':'
            << position
            << ": "
            << v
            << " ("
            << c
            << ")\n";
    }

    return 0;
}

int main(int argc, char* argv[]) {
    bool positional_arguments { true };

    bool infilename_set { false };
    std::string infilename {};

    bool outfilename_set { false };
    std::string outfilename {};

    mode mode { mode::default_mode };

    for (int i = 1; i < argc; ++i) {
        const std::string arg { argv[i] };

        if (arg.empty()) {
            /* ignore empty arguments */
            continue;
        }

        if (positional_arguments && arg.find('-') == 0) {
            if (arg == "-h" || arg == "--help") {
                usage(std::cout);
                std::exit(0);
            }

            if (arg == "-o" || arg == "--out-file") {
                if (outfilename_set) {
                    std::cerr << "Error: Multiple output files\n\n";
                    usage(std::cerr);
                    std::exit(1);
                }

                if (i == argc - 1) {
                    std::cerr
                        << "Error: Missing parameter to "
                        << arg
                        << "\n\n";
                    usage(std::cerr);
                    std::exit(1);
                }

                i += 1;
                outfilename = argv[i];
                outfilename_set = true;
                continue;
            }

            if (arg == "--only-io") {
                if (mode != mode::default_mode) {
                    std::cerr << "Error: Invalid mode combination\n";
                    std::exit(1);
                }

                mode = mode::only_io;
                continue;
            }

            std::cerr << "Error: Unknown parameter \"" << arg << "\"\n\n";
            usage(std::cerr);
            std::exit(1);
        }

        if (infilename_set) {
            std::cerr << "Error: Multiple input files\n\n";
            usage(std::cerr);
            std::exit(1);
        }

        infilename = arg;
        infilename_set = true;
    }

    std::ifstream instream;
    arabilis::Reader reader = [&]() {
        if (infilename_set) {
            instream.open(infilename, std::ios::binary);
            if (!instream) {
                std::cerr
                    << "Error: Unable to open input file \""
                    << infilename
                    << "\"\n";
                exit(1);
            }
            return arabilis::Reader { infilename, instream };
        } else {
            return arabilis::Reader { "interactive", std::cin };
        }
    }();

    std::ofstream outstream;
    arabilis::Writer writer = [&]() {
        if (outfilename_set) {
            outstream.open(outfilename, std::ios::binary);
            if (!outstream) {
                std::cerr
                    << "Error: Unable to open output file \""
                    << outfilename
                    << "\"\n";
                exit(1);
            }
            return arabilis::Writer { outstream };
        } else {
            return arabilis::Writer { std::cout };
        }
    }();

    if (mode == mode::only_io) {
        return mode_only_io(reader, writer);
    }

    return 1;
}
