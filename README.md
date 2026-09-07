# Project Meow
Meow is a toy kernel written in C (and some ASM) that can run on x86-64, AArch 64, RISC-V 64 and LoongArch 64 architectures.

The speed at which I update this project depends on how well I slept last night and what I ate for breakfast today.

# Language
Currently, only C and ASM are accepted; languages ​​such as Rust and C++ are not being considered.

# Note
Due to a mistake I made with Git, all previous commit records were lost.

# Build
Meow is compiled using CMake and Clang (CMake 3.30+, Clang 17+).

x86_64 Version:
```
mkdir build && cd build
cmake .. -DARCH=x86_64 && make -j$(nproc)
```

AArch 64 Version:
```
mkdir build && cd build
cmake .. -DARCH=aarch64 && make -j$(nproc)
```

RISC-V 64 Version:
```
mkdir build && cd build
cmake .. -DARCH=riscv64 && make -j$(nproc)
```

LoongArch 64 Version:
```
mkdir build && cd build
cmake .. -DARCH=loongarch64 && make -j$(nproc)
```

Thanks to the use of Clang, you can even perform cross-compilation directly on an Android phone :)

# TODO
The following are the goals of Project Meow.
## Short-term goals
- [x] Limine compatibility
- [x] Physical Memory Manager
- [ ] Enable memory paging (identical mapping)
- [x] The basic `stopcpu()` and `panic()` functions
- [ ] IDT and Interrupt Handling
- [ ] Memory repaging and mapping the kernel to the high half of memory
- [ ] Physical Memory Manager
- [ ] Virtual Memory Manager
- [ ] Memory Allocator

## Long-term goals
- [ ] SMP support
- [ ] Keyboard interrupt (APIC) and PS/2 driver
- [ ] Virtual File System
- [ ] PATA and SATA drivers
- [ ] File system
- [ ] Enter user space
- [ ] Implement some system calls

## Almost impossible goal
- [ ] Have basic toolchain

# License
[GNU AGPLv3](https://www.gnu.org/licenses/) or later

<img src="https://upload.wikimedia.org/wikipedia/commons/0/06/AGPLv3_Logo.svg" alt="GNU AGPLv3 Logo" style="width: 200px; height: auto;">
