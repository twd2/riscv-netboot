#ifndef _FIRMWARE_ASM_H_
#define _FIRMWARE_ASM_H_

#include "stdint.h"

static inline void fence()
{
    asm volatile ("fence");
}

static inline void fence_i()
{
    asm volatile ("fence.i");
}

static inline uint64_t mtval()
{
    uint64_t v;
    asm volatile ("csrr %0, mtval" : "=r"(v));
    return v;
}

static inline uint64_t mcause()
{
    uint64_t v;
    asm volatile ("csrr %0, mcause" : "=r"(v));
    return v;
}

static inline uint64_t mepc()
{
    uint64_t v;
    asm volatile ("csrr %0, mepc" : "=r"(v));
    return v;
}

static inline uint64_t rdcycle()
{
    uint64_t v;
    asm volatile ("rdcycle %0" : "=r"(v));
    return v;
}

#define CLOCK_FREQ 125000000ul

#define STACK_OFFSET (0)

#define DRAM_BASE 0x80000000ul
#define DRAM_SIZE 0x40000000ul

extern void enter_bootloader();
extern void enter_kernel(void *dtb, void *kernel_entry);

#endif
