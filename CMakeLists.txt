#  SPDX-License-Identifier: AGPL-3.0-or-later
#  Copyright 2026 KasuganoMeow
#  This file is part of the Meow.
#
#  The Meow is free software: you can redistribute it and/or
#  modify it under the terms of the GNU Affero General Public
#  License as published by the Free Software Foundation, either
#  version 3 of the License, or (at your option) any later version.
#
#  The Meow is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public
#  License along with the Meow. If not, see
#  <https://www.gnu.org/licenses/>.
#
#  This file path: CMakeLists.txt
#
cmake_minimum_required(VERSION 3.30)
project(Meow C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_COMPILER clang)
set(CMAKE_ASM_COMPILER clang)

set(INTERNAL_COMPILE_FLAGS
    -Wall
    -Wextra
    -std=gnu23
    -ffreestanding
    -fno-stack-protector
    -fno-stack-check
    -fno-lto
    -fno-PIC
    -ffunction-sections
    -fdata-sections
)

if(ARCH STREQUAL "x86_64")
	set(ARCH_COMPILE_FLAGS
        -target x86_64-unknown-none-elf
        -m64
        -march=x86-64
        -mabi=sysv
        -mno-80387
        -mno-mmx
        -mno-sse
        -mno-sse2
        -mno-red-zone
        -mcmodel=kernel
	)
    set(ARCH_LINK_FLAGS
        -Wl,-m,elf_x86_64
    )
elseif(ARCH STREQUAL "aarch64")
	set(ARCH_COMPILE_FLAGS
        -target aarch64-unknown-none-elf
        -march=armv8-a
        -mcmodel=large
        -mno-outline-atomics
        -mgeneral-regs-only
        -mno-omit-leaf-frame-pointer
	)
    set(ARCH_LINK_FLAGS
        -Wl,-m,aarch64elf
    )
elseif(ARCH STREQUAL "riscv64")
    set(ARCH_COMPILE_FLAGS
        -target riscv64-unknown-none-elf
        -march=rv64imafdc
        -mabi=lp64d
        -mcmodel=medany
        -mno-relax
    )
    set(ARCH_LINK_FLAGS
        -Wl,-m,elf64lriscv
    )
elseif(ARCH STREQUAL "loongarch64")
    set(ARCH_COMPILE_FLAGS
        -target loongarch64-unknown-none-elf
        -march=loongarch64
        -mabi=lp64d
        -mcmodel=extreme
        -mno-relax
    )
    set(ARCH_LINK_FLAGS
        -Wl,-m,elf64loongarch
    )
endif()

set(COMPILE_FLAGS
    -g
    -O2
    -pipe
    ${INTERNAL_COMPILE_FLAGS}
    ${ARCH_COMPILE_FLAGS}
)

set(LINK_FLAGS
    -fuse-ld=lld
    ${ARCH_LINK_FLAGS}
    -nostdlib
    -static
    -Wl,-z,max-page-size=0x1000
    -Wl,-z,noexecstack
    -Wl,--gc-sections
    -Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/kernel/arch/${ARCH}/linker.lds
)

add_executable(meow)

target_compile_options(meow PRIVATE ${COMPILE_FLAGS})
target_link_options(meow PRIVATE ${LINK_FLAGS})

add_subdirectory(kernel)
add_subdirectory(libk)
