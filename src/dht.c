#include <stdint.h>

#include "xmc_gpio.h"
#include "xmc_common.h"

#include "dht.h"
#include "delay.h"

#define TIMEOUT_LIMIT 500000

static int16_t return_err(int16_t err_type) {
    __enable_irq();
    return err_type;
}

int16_t read_temp(XMC_GPIO_PORT_t *const port, uint8_t pin) {
    uint8_t data[5] = {0};
    uint32_t timeout = 0;
    
    XMC_GPIO_SetOutputHigh(port, pin);
    delay_ms(10);

    XMC_GPIO_SetMode(port, pin, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    XMC_GPIO_SetOutputLow(port, pin);
    delay_ms(2);

    /* ------------ TIMED SEQUENCE START ------------ */
    __disable_irq();

    XMC_GPIO_SetMode(port, pin, XMC_GPIO_MODE_INPUT_PULL_UP);
    
    timeout = 0;
    while(XMC_GPIO_GetInput(port, pin) == 1) {
        if(timeout++ >= TIMEOUT_LIMIT) {
            return return_err(DHT_ERR);
        }
    }

    timeout = 0;
    while(XMC_GPIO_GetInput(port, pin) == 0) {
        if(timeout++ >= TIMEOUT_LIMIT) {
            return return_err(DHT_ERR);
        }
    }

    timeout = 0;
    while(XMC_GPIO_GetInput(port, pin) == 1) {
        if(timeout++ >= TIMEOUT_LIMIT) {
            return return_err(DHT_ERR);
        }
    }

    __enable_irq();
    /* ------------ TIMED SEQUENCE END ------------ */

    for (int i = 0; i < 40; i++) {
        timeout = 0;
        while(XMC_GPIO_GetInput(port, pin) == 0) {
            if(timeout++ >= TIMEOUT_LIMIT) {
                return return_err(DHT_ERR);
            }
        }

        uint32_t start_tick = SysTick->VAL;
        timeout = 0;
        
        while(XMC_GPIO_GetInput(port, pin) == 1) {
            if(timeout++ >= TIMEOUT_LIMIT) {
                return return_err(DHT_ERR);
            }
        }
        
        uint32_t end_tick = SysTick->VAL;
        uint32_t elapsed_ticks;
        
        if (start_tick >= end_tick) {
            elapsed_ticks = start_tick - end_tick;
        } else {
            elapsed_ticks = start_tick + (SysTick->LOAD - end_tick);
        }
        
        uint32_t elapsed_us = elapsed_ticks / (SystemCoreClock / 1000000);

        if (elapsed_us > 40) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if (data[4] != checksum) {
        return return_err(DHT_CHKSUM_ERR);
    }

    int16_t temp_tenths = ((data[2] & 0x7F) << 8) | data[3];
    if (data[2] & 0x80) {
        temp_tenths = -temp_tenths;
    }

    return temp_tenths;
}
