# Ex Lutum

Ex Lutum (latin "from dirt") is my attempt to create a self-hosted compiler,
("ARABILIS") from scratch, with the tiniest amount of binary "seed" file to
bootstrap the toolchain.

To achieve this goal, several intermediate languages are used. They are named,
rather unimaginative, "HEX", "LABEL", and "MACRO". Each extends the
capabilities of the previous languages:

* "HEX" allows for translation of hexadecimal values into binary, similar to
  the most simple use case of
  "[xxd](https://github.com/vim/vim/blob/master/src/xxd)".
* "LABEL" extends "HEX" so that the definition and invokation of labels are
  possible and e.g. addresses of functions do not need to be calculated by
  hand anymore.
* "MACRO" extends "LABEL" by the ability to name sequences of bytes and allows
  for the creation of a primitive assembly language.
* "ARABILIS" at last is a full general purpose language. Primitive and without
  the blessings of features such as type checking or a proper standard library,
  but powerful enough to do file I/O, dynamic memory allocation and embedding
  pre-compiled machine instructions.

Ex Lutum tries to focus on auditability, i.e. its source code shall be as
straight-forward and unsurprising as possible, sacrificing efficiency if
necessary. This is done to create a chain of trust from the initial seed binary
to the final compiler, that no malicious code was inserted (see "[Reflections
on Trusting Trust](https://users.ece.cmu.edu/~ganger/712.fall02/papers/p761-thompson.pdf)"
by Ken Thompson). Hence the use of e.g. `mov eax, 0` instead of the more
efficient `xor eax, eax` or `while(1) { if (...) break; ... }` instead of
`while (...) {...}` to stay more faithful to the assembly version of the same
program and similar examples.

# Building

## Build Requirements

Theoretically none, except for a shell, the ability to execute x86_32 ELF
executables and perform Linux compatible syscalls.

In reality, to build all intermediate programs, reenact the development of the
intermediate compilers and run the test suite:

* [cmake](https://cmake.org/)
* [nasm](https://nasm.us/)
* [gcc](https://gcc.gnu.org/) or [clang](https://clang.llvm.org/)
* [python3](https://www.python.org/)

## Building

The easy way of building Arabilis is:

```
mkdir build && cd build
cmake ..
make -j $(nproc)
```

The purist way of building Arabilis would be to compile one compiler with the
previous one, but that does not work yet.

# Examples

See the [examples](./examples) directory for examples of programs written in
the aforementioned intermediate languages and Arabilis.

# Resources

* https://www.bootstrappable.org/
* https://reproducible-builds.org/

# License and Contribution

While I am more than grateful for feedback and comments, I currently do not
accept code contributions, as the premise of this project is for me to prove to
myself that I can write a compiler, from scratch, myself. This will probably
change, once I consider this project complete and megalomania drives me to
implement a C compiler in Arabilis...

All code in this repository is licensed under the terms of GPL v3 or later.
