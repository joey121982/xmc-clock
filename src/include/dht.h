#pragma once

#include <stdint.h>

#include "xmc_gpio.h"

#define DHT_ERR         0xFFFFFFFF
#define DHT_CHKSUM_ERR  0xFFFFFFFE

int16_t read_temp(XMC_GPIO_PORT_t *const port, uint8_t pin);