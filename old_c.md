# C language

Both .c & .h files are to carry the same syntax C code.

- .c is meant to hold actual source code
- .h files hold interfaces, declarations & function signatures describing a compiled binary so that it can be used by:
  - Programmers to use (link against) that binary library in their source files
  - Compilers to resolve symbols and place the correct binary offsets

## Overall Paradigm

In C you are writing 'description' of the process that will be scheduled & run by the OS directly on CPU hardware. The description of a process is called a **program**, which has to be in a specific binary format so that the OS accepts it. But we do not write the binary directly, we write our C program in the format specified by the C standard and implemented by the compiler toolchain, and then we can compile it to multiple targets (binary formats) to run on several platforms (web, x64, arm64, multiple OSes)

The C standard is a very minimal & practical abstraction over assembly language. It is not a very high abstraction (like C++ & Rust), hence they do not compete/replace it. In fact, no language can replace C because its ABI (Application Binary Interface) is now a de facto standard (which the OS specifies respective of its CPU ISA) for programs in multiple languages (Python, JS, Rust, C++, and others) to talk with each other. In zero-cost abstraction languages like C++ & Rust, C is the lowest common denominator and agreed upon ABI, so they rely on it to communicate between their programs at binary level.

The biggest advantage is portability; one source compiled to multiple targets. Suppose you have to write a software artifact, like an OS. If you write it in for example x64 assembly, to run it on modern arm hardware, you would have to write millions of lines of code in another entire language. Obviously the logical design is built, so you have to somehow spend time translating the same logic in another CPU's instructions. In C, you can write your logic once and compile it to multiple targets.

Abstraction from the CPU hardware language gives another advantage: Bootstrapping. It is a very interesting & detailed topic, not to be discussed here.

## Thinking in C

C has 2 types:

- Bare-metal
- User-space

In bare metal C, you are doing nothing but literally grouping assembly instructions because otherwise it would be tedious. For example, you write:

```C
// Random code
int func(char c, bool b) {
    int[60] x;
    x[0] = 15;
}
```

Now these will be converted into assembly instructions very predictably. Functions follow the concept of **stack**. At binary, this function will just be a symbol (its name in ascii) stored pointing to an offset in the binary file which contains an assembly instruction followed by more, i just cannot explain it
