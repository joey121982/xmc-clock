#include "xmc_gpio.h"
#include "SEGGER_RTT.h"

#include <stdint.h>
#include <stdbool.h>

#define LED1_PORT XMC_GPIO_PORT1
#define LED1_PIN  0U

void delay(volatile uint32_t cycles) {
    while(cycles--) {
        __asm("nop");
    }
}

int main(void) {
    XMC_GPIO_CONFIG_t config = {
        .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
        .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
    };

    XMC_GPIO_Init(LED1_PORT, LED1_PIN, &config);

    while(1) {
        static bool led_status = 1;
        XMC_GPIO_ToggleOutput(LED1_PORT, LED1_PIN);

        SEGGER_RTT_printf(0, "Turned LED %s.\r\n", led_status ? "on" : "off");
        led_status = !led_status;

        delay(1000000);
    }
    
    return 0;
}