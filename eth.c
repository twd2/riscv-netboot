#include "eth.h"
#include "asm.h"
#include "serial.h"
#include "utils.h"
#include "printf.h"

int eth_send_not_begin = 1;

__attribute__ ((aligned (0x40))) dma_desc_t rx_desc;
__attribute__ ((aligned (0x40))) dma_desc_t tx_desc;

void init_eth()
{
    eth_send_not_begin = 1;

    reset_eth();

    puts("Initializing ethernet... ");

    rx_desc.next = &rx_desc;
    tx_desc.next = &tx_desc;

    uintptr_t ptr = (uintptr_t)&rx_desc;
    DMA_S2MM_CURDESC = (uint32_t)(ptr & 0xffffffff);
    DMA_S2MM_CURDESC_MSB = (uint32_t)((ptr >> 32) & 0xffffffff);
    DMA_S2MM_DMACR = 0;
    DMA_S2MM_DMACR |= DMA_DMACR_RS;
    while (DMA_S2MM_DMASR & DMA_DMASR_HALTED);

    ptr = (uintptr_t)&tx_desc;
    DMA_MM2S_CURDESC = (uint32_t)(ptr & 0xffffffff);
    DMA_MM2S_CURDESC_MSB = (uint32_t)((ptr >> 32) & 0xffffffff);
    DMA_MM2S_DMACR = 0;
    DMA_MM2S_DMACR |= DMA_DMACR_RS;
    while (DMA_MM2S_DMASR & DMA_DMASR_HALTED);

    puts("OK\r\n");
}

void reset_eth()
{
    puts("Resetting ethernet... ");

    ETH_RCW1R |= ETH_RCW1R_RX;
    ETH_TCR |= ETH_TCR_TX;

    ETH_RCW1R |= ETH_RCW1R_RST;
    while (ETH_RCW1R & ETH_RCW1R_RST);
    ETH_TCR |= ETH_TCR_RST;
    while (ETH_TCR & ETH_TCR_RST);

    DMA_S2MM_DMACR = 0;
    DMA_S2MM_DMACR |= DMA_DMACR_RESET;
    while (DMA_S2MM_DMACR & DMA_DMACR_RESET);
    DMA_MM2S_DMACR = 0;
    DMA_MM2S_DMACR |= DMA_DMACR_RESET;
    while (DMA_MM2S_DMACR & DMA_DMACR_RESET);

    puts("OK\r\n");
}

void eth_begin_dma_recv(void *data, uint32_t len)
{
    rx_desc.buff = data;
    rx_desc.control = len & DMA_DESC_CONTROL_LENGTH_MASK;
    rx_desc.status = 0;

    uintptr_t ptr = (uintptr_t)&rx_desc;
    DMA_S2MM_TAILDESC = (uint32_t)(ptr & 0xffffffff);
    fence();
    DMA_S2MM_TAILDESC_MSB = (uint32_t)((ptr >> 32) & 0xffffffff);
}

void eth_begin_dma_send(void *data, uint32_t len)
{
    eth_send_not_begin = 0;
    tx_desc.buff = data;
    tx_desc.control = (len & DMA_DESC_CONTROL_LENGTH_MASK)
                      | DMA_DESC_CONTROL_TXSOF | DMA_DESC_CONTROL_TXEOF;
    tx_desc.status = 0;

    uintptr_t ptr = (uintptr_t)&tx_desc;
    DMA_MM2S_TAILDESC = (uint32_t)(ptr & 0xffffffff);
    fence();
    DMA_MM2S_TAILDESC_MSB = (uint32_t)((ptr >> 32) & 0xffffffff);
}
