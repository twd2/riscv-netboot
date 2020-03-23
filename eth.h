#ifndef _FIRMWARE_ETH_H_
#define _FIRMWARE_ETH_H_

#include "stdint.h"

#define DMA_BASE 0x60300000
#define DMA_MM2S_DMACR (*(volatile uint32_t *)(DMA_BASE + 0x0))
#define DMA_MM2S_DMASR (*(volatile uint32_t *)(DMA_BASE + 0x4))
#define DMA_MM2S_CURDESC (*(volatile uint32_t *)(DMA_BASE + 0x8))
#define DMA_MM2S_CURDESC_MSB (*(volatile uint32_t *)(DMA_BASE + 0xc))
#define DMA_MM2S_TAILDESC (*(volatile uint32_t *)(DMA_BASE + 0x10))
#define DMA_MM2S_TAILDESC_MSB (*(volatile uint32_t *)(DMA_BASE + 0x14))
#define DMA_MM2S_SA (*(volatile uint32_t *)(DMA_BASE + 0x18))
#define DMA_MM2S_SA_MSB (*(volatile uint32_t *)(DMA_BASE + 0x1c))
#define DMA_MM2S_LENGTH (*(volatile uint32_t *)(DMA_BASE + 0x28))
#define DMA_S2MM_DMACR (*(volatile uint32_t *)(DMA_BASE + 0x30))
#define DMA_S2MM_DMASR (*(volatile uint32_t *)(DMA_BASE + 0x34))
#define DMA_S2MM_CURDESC (*(volatile uint32_t *)(DMA_BASE + 0x38))
#define DMA_S2MM_CURDESC_MSB (*(volatile uint32_t *)(DMA_BASE + 0x3c))
#define DMA_S2MM_TAILDESC (*(volatile uint32_t *)(DMA_BASE + 0x40))
#define DMA_S2MM_TAILDESC_MSB (*(volatile uint32_t *)(DMA_BASE + 0x44))
#define DMA_S2MM_DA (*(volatile uint32_t *)(DMA_BASE + 0x48))
#define DMA_S2MM_DA_MSB (*(volatile uint32_t *)(DMA_BASE + 0x4c))
#define DMA_S2MM_LENGTH (*(volatile uint32_t *)(DMA_BASE + 0x58))

#define DMA_DMACR_RS 1u
#define DMA_DMACR_RESET 4u
#define DMA_DMASR_HALTED 1u
#define DMA_DMASR_IDLE 2u

#define DMA_DESC_CONTROL_LENGTH_MASK ((1u << 26) - 1)
#define DMA_DESC_CONTROL_TXSOF (1u << 27)
#define DMA_DESC_CONTROL_TXEOF (1u << 26)
#define DMA_DESC_STATUS_LENGTH_MASK ((1u << 26) - 1)
#define DMA_DESC_STATUS_ERROR_MASK 0x70000000u
#define DMA_DESC_STATUS_COMPLETE 0x80000000u

typedef struct dma_desc dma_desc_t;
struct dma_desc
{
    dma_desc_t *next;
    void *buff;
    uint64_t reserved;
    uint32_t control;
    uint32_t status;
    uint32_t app[5];
    uint32_t reserved1[3];
};

#define ETH_BASE 0x60200000
#define ETH_RCW1R (*(volatile uint32_t *)(ETH_BASE + 0x404))
#define ETH_TCR (*(volatile uint32_t *)(ETH_BASE + 0x408))

#define ETH_RCW1R_RST 0x80000000u
#define ETH_RCW1R_RX 0x10000000u
#define ETH_TCR_RST 0x80000000u
#define ETH_TCR_TX 0x10000000u

void init_eth();
void reset_eth();

void eth_begin_dma_recv(void *data, uint32_t len);
void eth_begin_dma_send(void *data, uint32_t len);

static inline int eth_poll_recv()
{
    return DMA_S2MM_DMASR & DMA_DMASR_IDLE;
}

extern int eth_send_not_begin;

static inline int eth_poll_send()
{
    return eth_send_not_begin || (DMA_MM2S_DMASR & DMA_DMASR_IDLE);
}

extern dma_desc_t rx_desc;
extern dma_desc_t tx_desc;

static inline uint16_t eth_last_recv_len()
{
    return rx_desc.status & DMA_DESC_STATUS_LENGTH_MASK;
}

#endif
