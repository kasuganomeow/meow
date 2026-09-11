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
 * This file path: kernel/arch/aarch64/int/trap.c
 */
#include <int/trap.h>

#include <con/kprint.h>
#include <con/con.h>
#include <panic.h>

extern char trap_vector_table[];

struct trap_frame {
    uint64_t x[31];
    uint64_t vector;
    uint64_t pc;
    uint64_t pstate;
    uint64_t fault_addr;
    uint64_t esr;
};

static trap_handler_fn_t IRQ_HANDLERS[256] = {0};

static const char *const AARCH64_EXCEPTIONS[64] = {
    [0x00] = "Unknown / Uncategorized",
    [0x01] = "Trapped WFI/WFE Instruction",
    [0x07] = "Access to SVE/SIMD/FP Trapped",
    [0x0E] = "Illegal Execution State",
    [0x15] = "SVC Instruction Call (AArch64)",
    [0x18] = "Trapped MSR/MRS Access",
    [0x20] = "Instruction Abort (Lower EL)",
    [0x21] = "Instruction Abort (Current EL)",
    [0x22] = "PC Alignment Fault",
    [0x24] = "Data Abort (Lower EL)",
    [0x25] = "Data Abort (Current EL)",
    [0x26] = "SP Alignment Fault",
    [0x28] = "Trapped FP Exception (AArch64)",
    [0x2F] = "SError Interrupt",
    [0x30] = "Breakpoint (Lower EL)",
    [0x31] = "Breakpoint (Current EL)",
    [0x32] = "Software Step (Lower EL)",
    [0x33] = "Software Step (Current EL)",
    [0x34] = "Watchpoint (Lower EL)",
    [0x35] = "Watchpoint (Current EL)",
    [0x3C] = "BRK Instruction Execution"
};

void trap_init(void) {
    trap_init_percpu();
}

void trap_init_percpu(void) {
    asm volatile("msr vbar_el1, %0; isb" : : "r"(trap_vector_table) : "memory");
}

void trap_register_handler(uint32_t vector, trap_handler_fn_t handler) {
    if (vector < 256) IRQ_HANDLERS[vector] = handler;
}

void trap_handler(trap_frame_t *frame) {
    uint32_t ec = (frame->esr >> 26) & 0x3F;

    if (frame->vector == 0 || frame->vector == 4 || frame->vector == 8) {
        const char *name = (ec < 64 && AARCH64_EXCEPTIONS[ec]) ? \
            AARCH64_EXCEPTIONS[ec] : "Unknown Synch Exception";

        kprint("\nCPU EXCEPTION!\n");
        kprint(
            "FATAL ERROR: %s (ENTRY: %u, EC: 0x%02x, ISS: 0x%06x)\n",
            name, (uint32_t)frame->vector, ec,
            (uint32_t)(frame->esr & 0x1FFFFFF)
        );
        kprint(
            "MODE : %s\n",
            trap_frame_is_user(frame) ? "USER (EL0)" : "KERNEL (EL1)"
        );

        if (ec == 0x24 || ec == 0x25 || ec == 0x20 || ec == 0x21) {
            bool is_write = (frame->esr >> 6) & 1;
            uint32_t dfsc = frame->esr & 0x3F;

            kprint("\n[Page Fault / Data Abort Details]:\n");
            kprint(
                "  Faulting Virtual Address (FAR_EL1): 0x%016lx\n",
                frame->fault_addr
            );
            kprint(
                "  Cause: [%s] [%s] [DFSC/IFSC: 0x%02x]\n",
                is_write ? "Write Access" : "Read Access",
                trap_frame_is_user(frame) ? "User Mode" : "Supervisor Mode",
                dfsc
            );
        } else {
            kprint("\n");
        }

        kprint("PC:   0x%016lx  ", frame->pc);
        kprint("SP:  0x%016lx  ", (uint64_t)frame);
        kprint("PSTATE: 0x%016lx\n", frame->pstate);

        kprint("X0:   0x%016lx  ", frame->x[0]);
        kprint("X1:  0x%016lx  ", frame->x[1]);
        kprint("X2:     0x%016lx\n", frame->x[2]);

        kprint("X3:   0x%016lx  ", frame->x[3]);
        kprint("X4:  0x%016lx  ", frame->x[4]);
        kprint("X5:     0x%016lx\n", frame->x[5]);

        kprint("X6:   0x%016lx  ", frame->x[6]);
        kprint("X7:  0x%016lx  ", frame->x[7]);
        kprint("X8:     0x%016lx\n", frame->x[8]);

        kprint("X9:   0x%016lx  ", frame->x[9]);
        kprint("X10: 0x%016lx  ", frame->x[10]);
        kprint("X11:    0x%016lx\n", frame->x[11]);

        kprint("X12:  0x%016lx  ", frame->x[12]);
        kprint("X13: 0x%016lx  ", frame->x[13]);
        kprint("X14:    0x%016lx\n", frame->x[14]);

        kprint("X15:  0x%016lx  ", frame->x[15]);
        kprint("X16: 0x%016lx  ", frame->x[16]);
        kprint("X17:    0x%016lx\n", frame->x[17]);

        kprint("X18:  0x%016lx  ", frame->x[18]);
        kprint("X19: 0x%016lx  ", frame->x[19]);
        kprint("X20:    0x%016lx\n", frame->x[20]);

        kprint("X21:  0x%016lx  ", frame->x[21]);
        kprint("X22: 0x%016lx  ", frame->x[22]);
        kprint("X23:    0x%016lx\n", frame->x[23]);

        kprint("X24:  0x%016lx  ", frame->x[24]);
        kprint("X25: 0x%016lx  ", frame->x[25]);
        kprint("X26:    0x%016lx\n", frame->x[26]);

        kprint("X27:  0x%016lx  ", frame->x[27]);
        kprint("X28: 0x%016lx  ", frame->x[28]);
        kprint("X29:    0x%016lx\n", frame->x[29]);

        kprint("X30:  0x%016lx  ", frame->x[30]);
        kprint("FAR: 0x%016lx  ", frame->fault_addr);
        kprint("ESR:    0x%016lx\n", frame->esr);

        panic("CPU EXCEPTION: %s", name);
    }

    if (IRQ_HANDLERS[frame->vector]) {
        IRQ_HANDLERS[frame->vector](frame);
    }

    /*
     * ARM GIC 中断控制器 EOI 框架预留:
     * 实现 GICv2/v3 后，在此处写入 GICC_EOIR 或 ICC_EOIR1_EL1
     * 进行中断结束确认：
     * gic_eoi(irq_number);
     */
}

uintptr_t trap_frame_get_ip(const trap_frame_t *frame) {
    return frame->pc;
}

uintptr_t trap_frame_get_sp(const trap_frame_t *frame) {
    return (uintptr_t)frame;
}

bool trap_frame_is_user(const trap_frame_t *frame) {
    return (frame->pstate & 0x0F) == 0x00;
}

[[gnu::noreturn]]
void enter_userspace(uintptr_t user_entry, uintptr_t user_sp) {
    asm volatile(
        "msr elr_el1, %0\n\t"
        "msr spsr_el1, xzr\n\t"
        "msr sp_el0, %1\n\t"
        "eret\n\t"
        : : "r"(user_entry), "r"(user_sp) : "memory"
    );
    __builtin_unreachable();
}
