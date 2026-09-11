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
 * This file path: kernel/arch/loongarch64/int/trap.c
 */
#include <int/trap.h>
#include <con/kprint.h>
#include <con/con.h>
#include <panic.h>

extern char trap_vector_table[];

struct trap_frame {
    uint64_t ra, tp;
    uint64_t r4, r5, r6, r7, r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15, r16, r17, r18, r19;
    uint64_t r20, r21, r22, r23, r24, r25, r26, r27;
    uint64_t r28, r29, r30, r31;
    uint64_t era;
    uint64_t prmd;
    uint64_t estat;
    uint64_t badv;
    uint64_t sp;
};

static trap_handler_fn_t IRQ_HANDLERS[256] = {0};

static const char *const LOONGARCH64_EXCEPTIONS[32] = {
    [0x00] = "INT: External Interrupt",
    [0x01] = "PIL: Page Invalid for Load",
    [0x02] = "PIS: Page Invalid for Store",
    [0x03] = "PIF: Page Invalid for Fetch",
    [0x04] = "PME: Page Modification Exception",
    [0x05] = "FPD: Floating-Point Disabled",
    [0x06] = "SXD: 128-bit Vector Disabled",
    [0x07] = "ASXD: 256-bit Vector Disabled",
    [0x08] = "FPE: Floating-Point Exception",
    [0x09] = "WPE: Watchpoint Exception",
    [0x0A] = "ADE: Address Error",
    [0x0B] = "ALE: Address Alignment Error",
    [0x0C] = "SYS: System Call",
    [0x0D] = "BRK: Breakpoint",
    [0x0E] = "INE: Instruction Non-Existent",
    [0x0F] = "IPE: Instruction Privilege Error",
    [0x10] = "FPD: Floating-Point Data Error",
    [0x11] = "TLBR: TLB Refill Exception"
};

void trap_init(void) {
    trap_init_percpu();
}

void trap_init_percpu(void) {
    asm volatile(
        "csrwr %0, 0xc"
        : : "r"((uintptr_t)trap_vector_table)
        : "memory"
    );
}

void trap_register_handler(uint32_t vector, trap_handler_fn_t handler) {
    if (vector < 256) IRQ_HANDLERS[vector] = handler;
}

void trap_handler(trap_frame_t *frame) {
    uint32_t ecode = (frame->estat >> 16) & 0x3F;

    if (ecode != 0) {
        const char *name = \
            (ecode < 32 && LOONGARCH64_EXCEPTIONS[ecode]) ? \
            LOONGARCH64_EXCEPTIONS[ecode] : "Unknown Exception";

        kprint("\nCPU EXCEPTION!\n");
        kprint("FATAL ERROR: %s (ECODE: 0x%02x, ESUBCODE: 0x%03x)\n",
               name, ecode, (uint32_t)((frame->estat >> 22) & 0x1FF));
        kprint(
            "MODE : %s\n",
            trap_frame_is_user(frame) ? "USER (PLV3)" : "KERNEL (PLV0)"
        );

        if (
            ecode == 0x01 || ecode == 0x02 || ecode == 0x03 ||
            ecode == 0x04 || ecode == 0x11
        ) {
            kprint("\n[Page Fault / TLB Details]:\n");
            kprint("  Faulting Virtual Address (BADV): 0x%016lx\n", frame->badv);
            kprint(
                "  Cause: [%s] [%s]\n",
                (ecode == 0x02 || ecode == 0x04) ? "Write / Store" : \
                    ((ecode == 0x01) ? "Read / Load" : "Instruction Fetch"),
                trap_frame_is_user(frame) ? "User (PLV3)" : "Kernel (PLV0)"
            );
        } else {
            kprint("\n");
        }

        kprint("ERA:  0x%016lx  ", frame->era);
        kprint("SP:   0x%016lx  ", frame->sp);
        kprint("PRMD:  0x%016lx\n", frame->prmd);

        kprint("RA:   0x%016lx  ", frame->ra);
        kprint("TP:   0x%016lx  ", frame->tp);
        kprint("R4:    0x%016lx\n", frame->r4);

        kprint("R5:   0x%016lx  ", frame->r5);
        kprint("R6:   0x%016lx  ", frame->r6);
        kprint("R7:    0x%016lx\n", frame->r7);

        kprint("R8:   0x%016lx  ", frame->r8);
        kprint("R9:   0x%016lx  ", frame->r9);
        kprint("R10:   0x%016lx\n", frame->r10);

        kprint("R11:  0x%016lx  ", frame->r11);
        kprint("R12:  0x%016lx  ", frame->r12);
        kprint("R13:   0x%016lx\n", frame->r13);

        kprint("R14:  0x%016lx  ", frame->r14);
        kprint("R15:  0x%016lx  ", frame->r15);
        kprint("R16:   0x%016lx\n", frame->r16);

        kprint("R17:  0x%016lx  ", frame->r17);
        kprint("R18:  0x%016lx  ", frame->r18);
        kprint("R19:   0x%016lx\n", frame->r19);

        kprint("R20:  0x%016lx  ", frame->r20);
        kprint("R21:  0x%016lx  ", frame->r21);
        kprint("R22:   0x%016lx\n", frame->r22);

        kprint("R23:  0x%016lx  ", frame->r23);
        kprint("R24:  0x%016lx  ", frame->r24);
        kprint("R25:   0x%016lx\n", frame->r25);

        kprint("R26:  0x%016lx  ", frame->r26);
        kprint("R27:  0x%016lx  ", frame->r27);
        kprint("R28:   0x%016lx\n", frame->r28);

        kprint("R29:  0x%016lx  ", frame->r29);
        kprint("R30:  0x%016lx  ", frame->r30);
        kprint("R31:   0x%016lx\n", frame->r31);

        kprint("ESTAT:0x%016lx  ", frame->estat);
        kprint("BADV: 0x%016lx\n", frame->badv);

        panic("CPU EXCEPTION: %s", name);
    }

    uint32_t is = frame->estat & 0x1FFF;
    if (IRQ_HANDLERS[is]) {
        IRQ_HANDLERS[is](frame);
    }

    /*
     * LoongArch ExtIOI 中断控制器 EOI 框架预留:
     * 如开启外部扩展中断，在此处向 IOCSR 写入完成确认
     * extioi_eoi(irq_num);
     */
}

uintptr_t trap_frame_get_ip(const trap_frame_t *frame) {
    return frame->era;
}

uintptr_t trap_frame_get_sp(const trap_frame_t *frame) {
    return frame->sp;
}

bool trap_frame_is_user(const trap_frame_t *frame) {
    return (frame->prmd & 0x03) == 3;
}

[[gnu::noreturn]]
void enter_userspace(uintptr_t user_entry, uintptr_t user_sp) {
    uint64_t prmd = (3 << 0) | (1 << 2);
    asm volatile(
        "csrwr %0, 0x6\n\t"
        "csrwr %1, 0x1\n\t"
        "move  $sp, %2\n\t"
        "ertn\n\t"
        : : "r"(user_entry), "r"(prmd), "r"(user_sp) : "memory"
    );
    __builtin_unreachable();
}
