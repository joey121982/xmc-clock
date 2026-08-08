#include <stdint.h>

#include "xmc_common.h"
#include "delay.h"

void delay_us(uint32_t microseconds) {
    uint32_t start_val = SysTick->VAL;
    uint32_t ticks_to_wait = (SystemCoreClock / 1000000) * microseconds;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait) {
        uint32_t current_val = SysTick->VAL;
        
        if (start_val >= current_val) {
            elapsed_ticks += (start_val - current_val);
        } else {
            /* SysTick timer has rolled over */
            elapsed_ticks += (start_val + (SysTick->LOAD - current_val));
        }
        
        start_val = current_val;
    }
}

void delay_ms(uint32_t milliseconds) {
    delay_us(1000 * milliseconds);
}