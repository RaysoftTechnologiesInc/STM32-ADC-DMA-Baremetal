#include "adc_dma.h"

#define GPIOAEN			(1U<<0)
#define ADC1EN			(1U<<8)
#define CR1_SCAN		(1U<<8)
#define CR1_SCAN		(1U<<8)
#define CR2_DDS			(1U<<9)
#define CR2_CONT		(1U<<1)
#define CR2_ADCON		(1U<<0)
#define CR2_SWSTART		(1U<<30)

#define DMA2EN				(1U<<22)
#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_SCR_PINC		(1U<<9)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_TCIE		(1U<<4)
#define DMA_SCR_TEIE		(1U<<2)
#define DMA_SFCR_DMDIS		(1U<<2)

uint16_t adc_raw_data[NUM_OF_CHANNELS];

void adc_dma_init(void)
{
    /****GPIO Config***/
    //Enable clock access to adc gpio pins port
    RCC->AHB1ENR |= GPIOAEN;

    /*set pins as input*/
    GPIOA->MODER |= 1U<<0;
    GPIOA->MODER |= 1U<<1;

    GPIOA->MODER |= 1U<<2;
    GPIOA->MODER |= 1U<<3;

    /***ADC Configuration***/
    RCC->APB2ENR |= ADC1EN;

    //set sequence length
    ADC1->SQR1  |= (1U<<20);
    ADC1->SQR1  &=~(1U<<21);
    ADC1->SQR1  &=~(1U<<22);
    ADC1->SQR1  &=~(1U<<23);

    //set sequence
    ADC1->SQR3 = (0U<<0) | (1U<<5);

    //Enable scan mode
    ADC1->CR1 = CR1_SCAN;

    //Select to use DMA
    ADC1->CR2 |= CR2_CONT | ADC_CR2_DMA | CR2_DDS;


    /***DMA Configuration***/
    //Enable clock access to DMA
    RCC->AHB1ENR |= DMA2EN;
    //disable stream
    DMA2_Stream0->CR &=~DMA_SCR_EN;
    //wait till disbled
    while((DMA2_Stream0->CR) & DMA_SCR_EN){}

    //enable circular mode
    DMA2_Stream0->CR   |= DMA_SCR_CIRC;

    //set mem size to half word
    DMA2_Stream0->CR |= 1U<<13;
    DMA2_Stream0->CR &=~ (1U<<14);

    //set periipheral size
    DMA2_Stream0->CR |= 1U<<11;
    DMA2_Stream0->CR &=~ (1U<<12);

    //enable memory addr increment
    DMA2_Stream0->CR |= DMA_SCR_MINC;

    //set peripheral addr
    DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));
    DMA2_Stream0->M0AR = (uint32_t)(&adc_raw_data);

    //set number of transfers
    DMA2_Stream0->NDTR = NUM_OF_CHANNELS;

    //Enable stream
    DMA2_Stream0->CR |= DMA_SCR_EN;

    /***ADC Configuration***/
    //Enable ADC
    ADC1->CR2 |= CR2_ADCON;

    ///start adc
    ADC1->CR2 |= CR2_SWSTART;



}