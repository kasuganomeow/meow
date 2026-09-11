/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* Copyright 2026 KasuganoMeow
   This file is part of the Meow.

   The Meow is free software: you can redistribute it and/or
   modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either
   version 3 of the License, or (at your option) any later version.

   The Meow is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public
   License along with the Meow. If not, see
   <https://www.gnu.org/licenses/>. */
/*
 * This file path: kernel/arch/riscv64/int/trap.c
 */
#include <int/trap.h>
#include <con/kprint.h>
#include <con/con.h>
#include <panic.h>

extern char trap_vector_table[];

struct trap_frame {
    uint64_t ra, gp, tp, t0, t1, t2, s0, s1;
    uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
    uint64_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    uint64_t t3, t4, t5, t6;
    uint64_t sepc;
    uint64_t sstatus;
    uint64_t scause;
    uint64_t stval;
    uint64_t sp;
};

static trap_handler_fn_t IRQ_HANDLERS[256] = {0};

static const char *const RISCV64_EXCEPTIONS[] = {
    [0]  = "Instruction Address Misaligned",
    [1]  = "Instruction Access Fault",
    [2]  = "Illegal Instruction",
    [3]  = "Breakpoint",
    [4]  = "Load Address Misaligned",
    [5]  = "Load Access Fault",
    [6]  = "Store/AMO Address Misaligned",
    [7]  = "Store/AMO Access Fault",
    [8]  = "Environment Call from U-mode",
    [9]  = "Environment Call from S-mode",
    [12] = "Instruction Page Fault",
    [13] = "Load Page Fault",
    [15] = "Store/AMO Page Fault"
};

void trap_init(void) {
    trap_init_percpu();
}

void trap_init_percpu(void) {
    asm volatile("csrw stvec, %0" : : "r"((uintptr_t)trap_vector_table) : "memory");
}

void trap_register_handler(uint32_t vector, trap_handler_fn_t handler) {
    if (vector < 256) IRQ_HANDLERS[vector] = handler;
}

void trap_handler(trap_frame_t *frame) {
    bool is_interrupt = ((int64_t)frame->scause < 0);
    uint64_t code = frame->scause & 0x7FFFFFFFFFFFFFFFULL;

    if (!is_interrupt) {
        const char *name = (code < 24 && RISCV64_EXCEPTIONS[code]) ? RISCV64_EXCEPTIONS[code] : "Unknown Exception";

        kprint("\nCPU EXCEPTION!\n");
        kprint("FATAL ERROR: %s (EXCEPTION CODE: %lu)\n", name, code);
        kprint(
            "MODE : %s\n",
            trap_frame_is_user(frame) ? "USER (U-MODE)" : "KERNEL (S-MODE)"
        );

        if (code == 12 || code == 13 || code == 15) {
            kprint("\n[Page Fault Details]:\n");
            kprint("  Faulting Virtual Address (STVAL): 0x%016lx\n", frame->stval);
            kprint(
                "  Cause: [%s] [%s]\n",
                (code == 15) ? "Write / Store" : ((code == 13) ? \
                    "Read / Load" : "Instruction Fetch"),
                trap_frame_is_user(frame) ? "User Mode" : "Supervisor Mode"
            );
        } else {
            kprint("\n");
        }

        kprint("SEPC:    0x%016lx  ", frame->sepc);
        kprint("SP:      0x%016lx  ", frame->sp);
        kprint("SSTATUS: 0x%016lx\n", frame->sstatus);

        kprint("RA:      0x%016lx  ", frame->ra);
        kprint("GP:      0x%016lx  ", frame->gp);
        kprint("TP:      0x%016lx\n", frame->tp);

        kprint("T0:      0x%016lx  ", frame->t0);
        kprint("T1:      0x%016lx  ", frame->t1);
        kprint("T2:      0x%016lx\n", frame->t2);

        kprint("S0/FP:   0x%016lx  ", frame->s0);
        kprint("S1:      0x%016lx  ", frame->s1);
        kprint("A0:      0x%016lx\n", frame->a0);

        kprint("A1:      0x%016lx  ", frame->a1);
        kprint("A2:      0x%016lx  ", frame->a2);
        kprint("A3:      0x%016lx\n", frame->a3);

        kprint("A4:      0x%016lx  ", frame->a4);
        kprint("A5:      0x%016lx  ", frame->a5);
        kprint("A6:      0x%016lx\n", frame->a6);

        kprint("A7:      0x%016lx  ", frame->a7);
        kprint("S2:      0x%016lx  ", frame->s2);
        kprint("S3:      0x%016lx\n", frame->s3);

        kprint("S4:      0x%016lx  ", frame->s4);
        kprint("S5:      0x%016lx  ", frame->s5);
        kprint("S6:      0x%016lx\n", frame->s6);

        kprint("S7:      0x%016lx  ", frame->s7);
        kprint("S8:      0x%016lx  ", frame->s8);
        kprint("S9:      0x%016lx\n", frame->s9);

        kprint("S10:     0x%016lx  ", frame->s10);
        kprint("S11:     0x%016lx  ", frame->s11);
        kprint("T3:      0x%016lx\n", frame->t3);

        kprint("T4:      0x%016lx  ", frame->t4);
        kprint("T5:      0x%016lx  ", frame->t5);
        kprint("T6:      0x%016lx\n", frame->t6);

        kprint("SCAUSE:  0x%016lx  ", frame->scause);
        kprint("STVAL:   0x%016lx\n", frame->stval);

        panic("CPU EXCEPTION: %s", name);
    }

    if (IRQ_HANDLERS[code]) {
        IRQ_HANDLERS[code](frame);
    }

    /*
     * RISC-V PLIC / AIA 中断控制器 EOI 框架预留:
     * 如开启外部硬件中断，在此处通过 PLIC Claim/Complete
     * 寄存器写回完成：
     * plic_complete(hart_id, irq_id);
     */
}

uintptr_t trap_frame_get_ip(const trap_frame_t *frame) {
    return frame->sepc;
}

uintptr_t trap_frame_get_sp(const trap_frame_t *frame) {
    return frame->sp;
}

bool trap_frame_is_user(const trap_frame_t *frame) {
    return ((frame->sstatus >> 8) & 1) == 0;
}

[[gnu::noreturn]]
void enter_userspace(uintptr_t user_entry, uintptr_t user_sp) {
    uint64_t sstatus;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1ULL << 8);
    sstatus |=  (1ULL << 5);

    asm volatile(
        "csrw sstatus, %0\n\t"
        "csrw sepc, %1\n\t"
        "mv sp, %2\n\t"
        "sret\n\t"
        : : "r"(sstatus), "r"(user_entry), "r"(user_sp) : "memory"
    );
    __builtin_unreachable();
}
