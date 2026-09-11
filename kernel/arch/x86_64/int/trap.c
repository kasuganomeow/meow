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
 * This file path: kernel/arch/x86_64/int/trap.c
 */
#include <int/trap.h>
#include <con/kprint.h>
#include <con/con.h>
#include <mem/kalloc.h>
#include <panic.h>
//#include "lapic.h"
#include <string.h>

extern int8_t trap_vector_table[];

struct trap_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

typedef struct [[gnu::packed]] {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} gdt_entry_t;

typedef struct [[gnu::packed]] {
    uint16_t length;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  flags1;
    uint8_t  flags2;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
} tss_entry_t;

typedef struct [[gnu::packed]] {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} tss_t;

typedef struct [[gnu::packed]] {
    uint16_t limit;
    uint64_t base;
} desc_ptr_t;

typedef struct [[gnu::packed]] {
    gdt_entry_t entries[5];
    tss_entry_t tss;
} gdt_table_t;

typedef struct [[gnu::packed]] {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} idt_entry_t;

typedef struct {
    gdt_table_t gdt;
    tss_t       tss;
    desc_ptr_t  gdtr;
} percpu_gdt_ctx_t;

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_DATA   (0x18 | 3)
#define GDT_USER_CODE   (0x20 | 3)
#define GDT_TSS         0x28

static percpu_gdt_ctx_t g_boot_ctx;
static bool             g_bsp_initialized = false;

static idt_entry_t   IDT[256];
static desc_ptr_t    IDTR;

static trap_handler_fn_t IRQ_HANDLERS[256] = {0};

static const char *const X86_64_EXCEPTIONS[32] = {
    [0]  = "#DE: Divide Error",
    [1]  = "#DB: Debug",
    [2]  = "#NMI: Non-maskable Interrupt",
    [3]  = "#BP: Breakpoint",
    [4]  = "#OF: Overflow",
    [5]  = "#BR: Bound Range",
    [6]  = "#UD: Invalid Opcode",
    [7]  = "#NM: Device Not Available",
    [8]  = "#DF: Double Fault",
    [9]  = "Coprocessor Segment Overrun",
    [10] = "#TS: Invalid TSS",
    [11] = "#NP: Segment Not Present",
    [12] = "#SS: Stack Fault",
    [13] = "#GP: General Protection",
    [14] = "#PF: Page Fault",
    [15] = "Reserved Exception",
    [16] = "#MF: x87 Floating-Point",
    [17] = "#AC: Alignment Check",
    [18] = "#MC: Machine Check",
    [19] = "#XM: SIMD Exception",
    [20] = "#VE: Virtualization",
    [21] = "#CP: Control Protection",
    [30] = "#SX: Security Exception"
};

static void gdt_set_gate(gdt_entry_t *entry, uint8_t access, uint8_t flags) {
    entry->base_low = 0;
    entry->base_middle = 0;
    entry->base_high = 0;
    entry->limit_low = 0;
    entry->access = access;
    entry->granularity = flags & 0xF0;
}

static void tss_set_gate(tss_entry_t *entry, uint64_t addr, uint32_t size) {
    entry->length = (uint16_t)size;
    entry->base_low = (uint16_t)(addr & 0xFFFF);
    entry->base_middle = (uint8_t)((addr >> 16) & 0xFF);
    entry->flags1 = 0x89;
    entry->flags2 = 0x00;
    entry->base_high = (uint8_t)((addr >> 24) & 0xFF);
    entry->base_upper = (uint32_t)(addr >> 32);
    entry->reserved = 0;
}

static void gdt_load(desc_ptr_t *ptr) {
    asm volatile (
        "lgdt (%0)\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov $0x28, %%ax\n\t"
        "ltr %%ax\n\t"
        : : "r"(ptr) : "rax", "memory"
    );
}

static void idt_set_gate(uint8_t vector, void *isr, uint8_t flags) {
    uint64_t addr = (uint64_t)isr;
    IDT[vector].offset_low  = (uint16_t)(addr & 0xFFFF);
    IDT[vector].selector    = GDT_KERNEL_CODE;
    IDT[vector].ist         = 0;
    IDT[vector].type_attr   = flags;
    IDT[vector].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    IDT[vector].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    IDT[vector].reserved    = 0;
}

static void setup_gdt_context(percpu_gdt_ctx_t *ctx) {
    memset(&ctx->tss, 0, sizeof(tss_t));
    ctx->tss.iomap_base = sizeof(tss_t);

    gdt_set_gate(&ctx->gdt.entries[0], 0, 0);
    gdt_set_gate(&ctx->gdt.entries[1], 0x9A, 0x20);
    gdt_set_gate(&ctx->gdt.entries[2], 0x92, 0x00);
    gdt_set_gate(&ctx->gdt.entries[3], 0xF2, 0x00);
    gdt_set_gate(&ctx->gdt.entries[4], 0xFA, 0x20);
    tss_set_gate(&ctx->gdt.tss, (uint64_t)&ctx->tss, sizeof(tss_t) - 1);

    ctx->gdtr.limit = sizeof(gdt_table_t) - 1;
    ctx->gdtr.base  = (uint64_t)&ctx->gdt;
}

void trap_init(void) {
    for (int i = 0; i < 256; i++) {
        void *handler = (void *)(trap_vector_table + (i * 16));
        idt_set_gate(i, handler, 0x8E);
    }

    IDTR.limit = sizeof(IDT) - 1;
    IDTR.base  = (uint64_t)&IDT;

    trap_init_percpu();

    g_bsp_initialized = true;
}

void trap_init_percpu(void) {
    asm volatile("lidt %0" : : "m"(IDTR));

    if (!g_bsp_initialized) {
        setup_gdt_context(&g_boot_ctx);
        gdt_load(&g_boot_ctx.gdtr);
    } else {
        percpu_gdt_ctx_t *ap_ctx = (percpu_gdt_ctx_t *)kalloc(sizeof(percpu_gdt_ctx_t));
        if (!ap_ctx) {
            panic("trap_init_percpu: Failed to allocate dynamic GDT/TSS context for AP core");
        }

        setup_gdt_context(ap_ctx);
        gdt_load(&ap_ctx->gdtr);
    }
}

void trap_register_handler(uint32_t vector, trap_handler_fn_t handler) {
    if (vector < 256) {
        IRQ_HANDLERS[vector] = handler;
    }
}

void trap_handler(trap_frame_t *frame) {
    if (frame->vector < 32) {
        const char *name = X86_64_EXCEPTIONS[frame->vector];
        if (!name) name = "Unknown Exception";

        uint64_t cr2, cr3;
        asm volatile("mov %%cr2, %0" : "=r"(cr2));
        asm volatile("mov %%cr3, %0" : "=r"(cr3));

        kprint("\nCPU EXCEPTION!\n");
        kprint(
            "FATAL ERROR: %s (VECTOR: %u, ERR CODE: 0x%08lx)\n",
            name, (uint32_t)frame->vector, frame->error_code
        );
        kprint(
            "MODE : %s\n", trap_frame_is_user(frame) ? \
                "USER (RING 3)" : "KERNEL (RING 0)"
        );

        if (frame->vector == 14) {
            kprint("\n[Page Fault Details]:\n");
            kprint("  Faulting Virtual Address (CR2): 0x%016lx\n", cr2);
            kprint
            (
                "  Cause: [%s] [%s] [%s]%s\n",
                (frame->error_code & (1 << 0)) ? \
                    "Protection Violation" : "Non-Present",
                (frame->error_code & (1 << 1)) ? \
                    "Write" : "Read",
                (frame->error_code & (1 << 2)) ? \
                    "User Mode" : "Supervisor Mode",
                (frame->error_code & (1 << 4)) ? \
                    " [Instruction Fetch]" : ""
            );
        } else {
            kprint("\n");
        }

        //kprint("\nRegisters Dump (CPU #%u):\n", arch_cpu_id());
        kprint("RIP: 0x%016lx  ", frame->rip);
        kprint("RSP: 0x%016lx  ", frame->rsp);
        kprint("RFLAGS: 0x%016lx\n", frame->rsp, frame->rflags);

        kprint("RAX: 0x%016lx  ", frame->rax);
        kprint("RBX: 0x%016lx  ", frame->rbx);
        kprint("RCX:    0x%016lx\n", frame->rcx);

        kprint("RDX: 0x%016lx  ", frame->rdx);
        kprint("RSI: 0x%016lx  ", frame->rsi);
        kprint("RDI:    0x%016lx\n", frame->rdi);

        kprint("RBP: 0x%016lx  ", frame->rbp);
        kprint("R8:  0x%016lx  ", frame->r8);
        kprint("R9:     0x%016lx\n", frame->r9);

        kprint("R10: 0x%016lx  ", frame->r10);
        kprint("R11: 0x%016lx  ", frame->r11);
        kprint("R12:    0x%016lx\n", frame->r12);

        kprint("R13: 0x%016lx  ", frame->r13);
        kprint("R14: 0x%016lx  ", frame->r14);
        kprint("R15:    0x%016lx\n", frame->r15);

        kprint("CS:  0x%04lx  ", frame->cs);
        kprint("SS: 0x%04lx  ", frame->ss);
        kprint("CR2: 0x%016lx  ", cr2);
        kprint("CR3:    0x%016lx\n", cr3);

        panic("CPU EXCEPTION: %s", name);
    }

    if (IRQ_HANDLERS[frame->vector] != NULL) {
        IRQ_HANDLERS[frame->vector](frame);
    }

    //if (frame->vector != LAPIC_SPURIOUS_VECTOR) {
    //    lapic_eoi();
    //}
}

uintptr_t trap_frame_get_ip(const trap_frame_t *frame) {
    return frame->rip;
}

uintptr_t trap_frame_get_sp(const trap_frame_t *frame) {
    return frame->rsp;
}

bool trap_frame_is_user(const trap_frame_t *frame) {
    return (frame->cs & 0x03) == 3;
}

[[gnu::noreturn]]
void enter_userspace(uintptr_t user_entry, uintptr_t user_sp) {
    asm volatile(
        "cli\n\t"
        "mov %0, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "pushq %1\n\t"
        "pushq %2\n\t"
        "pushq $0x202\n\t"
        "pushq %3\n\t"
        "pushq %4\n\t"
        "iretq\n\t"
        :
        : "i"(GDT_USER_DATA),
          "i"(GDT_USER_DATA),
          "r"(user_sp),
          "i"(GDT_USER_CODE),
          "r"(user_entry)
        : "rax", "memory"
    );
    __builtin_unreachable();
}
