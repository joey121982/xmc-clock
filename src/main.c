#include <stdint.h>
#include <stdbool.h>

#include "xmc_gpio.h"
#include "SEGGER_RTT.h"

#include "delay.h"
#include "dht.h"

#define DHT_PIN XMC_GPIO_PORT0, 14

void SysTick_Handler(void) { /* empty */ }

void setup() {
    SysTick_Config(SystemCoreClock / 1000);

    XMC_GPIO_Init(DHT_PIN, (&(XMC_GPIO_CONFIG_t) {
        .mode         = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN,
        .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH
    }));
}

int main(void) {
    setup();
    SEGGER_RTT_Init();
    
    while(1) {
        uint32_t raw_data = read_temp(DHT_PIN);
        
        if (raw_data == DHT_ERR) {
            SEGGER_RTT_printf(0, "Sensor Error: Timeout fail\n");
        } else if (raw_data == DHT_CHKSUM_ERR) {
            SEGGER_RTT_printf(0, "Sensor Error: Checksum fail\n");
        } else {
            int32_t temp_tenths = (int32_t)raw_data;
            int32_t temp_int = temp_tenths / 10;
            int32_t temp_dec = temp_tenths % 10;
            
            if (temp_dec < 0) {
                temp_dec = -temp_dec;
            }

            SEGGER_RTT_printf(0, "Temperature: %d.%d C\n", temp_int, temp_dec);
        }
        
        delay_ms(3000); 
    }
    
    return 0;
}