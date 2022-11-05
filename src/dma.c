#include "dma.h"

#define DMA2EN          1U<<22
#define DMA_SCR_EN      1U<<0
#define DMA_SCR_MINC     1U<<10
#define DMA_SCR_PINC    1U<<9
#define DMA_SCR_TCIE    1U<<4
#define DMA_SCR_TEIE    1U<<2
#define DMA_SFCR_DMDIS  1U<<2

void dma2_mem2mem_config(void)
{
    //enable clock access to the dma module
    RCC->AHB1ENR |= DMA2EN;

    //disable dma stream
    DMA2_Stream0->CR = 0;

    //wait until stream is disabled
    while((DMA2_Stream0->CR & DMA_SCR_EN)){}

    /*Configure dma parameters*/
    
    //set mem size to half word
    DMA2_Stream0->CR |= 1U<<13;
    DMA2_Stream0->CR &=~ (1U<<14);

    //set peripheral data size to half word
    DMA2_Stream0->CR |= 1U<<11;
    DMA2_Stream0->CR &=~ (1U<<12);

    //enable memory addr increment
    DMA2_Stream0->CR |= DMA_SCR_MINC;

    //enable peripheral data incrememt
    DMA2_Stream0->CR |= DMA_SCR_PINC;

    //Select mem-to-mem transfer
    DMA2_Stream0->CR &=~ (1U<<6);
    DMA2_Stream0->CR |= 1U<<7;

    //enable transfer error interrupt
    DMA2_Stream0->CR |= DMA_SCR_TEIE;

    //enable transfer complete interrupt
    DMA2_Stream0->CR |= DMA_SCR_TCIE;

    //disable direct mode
    DMA2_Stream0->FCR |= DMA_SFCR_DMDIS;

    /*Set DMA FIFO Threshold*/
    DMA2_Stream0->FCR |= 1U<<0;
    DMA2_Stream0->FCR |= 1U<<1;

    //enable DMA Interrrupt
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void dma_transfer_start(uint32_t src_buff, uint32_t dest_buff, uint32_t len)
{
    //set peripheral address
    DMA2_Stream0->PAR = src_buff;

    //set memory address
    DMA2_Stream0->M0AR = dest_buff;

    //set transfer length
    DMA2_Stream0->NDTR = len;

    //enable DMA stream
    DMA2_Stream0->CR |= DMA_SCR_EN;
}