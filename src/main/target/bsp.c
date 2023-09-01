/*
 * bsp.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "bsp.h"
#include "uart.h"



#define DWT_LAR_UNLOCK_VALUE 0xC5ACCE55

// cycles per microsecond
static uint32_t usTicks = 0;
static uint32_t cpuClockFrequency = 0;

void cycleCounterInit(void)
{
  cpuClockFrequency = HAL_RCC_GetSysClockFreq();

  usTicks = cpuClockFrequency / 1000000;

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  __O uint32_t *DWTLAR = (uint32_t *)(DWT_BASE + 0x0FB0);
  *(DWTLAR) = DWT_LAR_UNLOCK_VALUE;

  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static volatile uint32_t msTicks = 0;

void HAL_SYSTICK_Callback(void)
{
	msTicks++;
}

uint32_t getCycleCounter(void)
{
    return DWT->CYCCNT;
}

int32_t clockCyclesToMicros(int32_t clockCycles)
{
    return clockCycles / usTicks;
}

// Note that this conversion is signed as this is used for periods rather than absolute timestamps
int32_t clockCyclesTo10thMicros(int32_t clockCycles)
{
    return 10 * clockCycles / (int32_t)usTicks;
}

uint32_t clockMicrosToCycles(uint32_t micros)
{
    return micros * usTicks;
}

void delay(uint32_t ms)
{
  HAL_Delay(ms);
}

uint32_t millis(void)
{
  return HAL_GetTick();
}

uint32_t micros(void)
{
	 register uint32_t ms, cycle_cnt;

	    do {
	        ms = msTicks;
	        cycle_cnt = SysTick->VAL;
	    } while (ms != msTicks);
	    return (ms * 1000) + (168 * 1000 - cycle_cnt) / 168; //168
}

void delayMicroseconds(uint32_t us)
{
    uint32_t now = micros();
    while (micros() - now < us);
}

int __io_putchar(int ch)
{
  uartWrite(_DEF_UART1, (uint8_t *)&ch, 1);
  return 1;
}