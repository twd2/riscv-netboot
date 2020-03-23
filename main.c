#include "asm.h"
#include "serial.h"
#include "printf.h"
#include "string.h"
#include "utils.h"
#include "task.h"
#include "eth.h"
#include "ip.h"
#include "dhcp.h"
#include "tftp.h"

void init_bss()
{
    extern uint8_t _bss_begin[];
    extern uint8_t _bss_end[];
    memset(_bss_begin, 0, (uintptr_t)_bss_end - (uintptr_t)_bss_begin);
}

void test_dram_1m()
{
    puts("Start DRAM testing (first 1M)... ");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + 0x100000; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = addr;
    }
    puts("write passed... ");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + 0x100000; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        if (*ptr != addr)
        {
            printf("addr failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("read passed.\r\n");
}

void test_dram()
{
    puts("Start DRAM testing...\r\n");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = 0x5555555555555555;
        if (*ptr != 0x5555555555555555)
        {
            printf("55 failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("55 passed.\r\n");

    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = 0xAAAAAAAAAAAAAAAA;
        if (*ptr != 0xAAAAAAAAAAAAAAAA)
        {
            printf("AA failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("AA passed.\r\n");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = -1;
        if (*ptr != -1)
        {
            printf("FF failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("FF passed.\r\n");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = 0;
        if (*ptr != 0)
        {
            printf("00 failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("00 passed.\r\n");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        *ptr = addr;
    }
    puts("write passed.\r\n");
    for (uintptr_t addr = DRAM_BASE; addr < DRAM_BASE + DRAM_SIZE; addr += 8)
    {
        volatile uint64_t *ptr = (uint64_t *)addr;
        if (*ptr != addr)
        {
            printf("addr failed at 0x%016lx\r\n", addr);
            break;
        }
    }
    puts("read passed.\r\n");
}

__attribute__ ((aligned (16))) uint8_t timer_1_stack[1024];
static struct task timer_1_task;
void timer_1_entry()
{
    while (1)
    {
        sleep(1 * CLOCK_FREQ);
    }
}

__attribute__ ((aligned (16))) uint8_t dma_buff[4096];
__attribute__ ((aligned (16))) uint8_t main_stack[1024];
static struct task main_task;
void main_entry()
{
    frame_t *const frame = (frame_t *)(dma_buff + ALIGN_OFFSET);
    const uint64_t len = sizeof(dma_buff) - ALIGN_OFFSET;

    while (1)
    {
        eth_begin_dma_recv((uint8_t *)frame + sizeof(frame_meta_t), len - sizeof(frame_meta_t));
        wait(WAIT_ETH_RECV);
        frame->meta.len = eth_last_recv_len();

        handle_frame(frame);
    }
}

void start(int hartid, void *dtb)
{
    fence();
    fence_i();

    init_bss();
    init_serial();
    init_eth();
    init_task();
    init_ip();

    //test_dram();
    printf("rdcycle: %lu\r\n", rdcycle());

    //create_task(&timer_1_task, (uintptr_t)timer_1_stack + sizeof(timer_1_stack) + STACK_OFFSET, timer_1_entry);
    create_task(&main_task, (uintptr_t)main_stack + sizeof(main_stack) + STACK_OFFSET, main_entry);
    init_dhcp();
    init_tftp();
    idle_entry();
    enter_bootloader();
}
