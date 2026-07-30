PROJECT = xmc_clock

# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Directories
SRC_DIR = src
INC_DIR = src/include
XMCLIB_DIR = mtb-xmclib-cat3
CMSIS_CORE_DIR = lib/CMSIS_5/CMSIS/Core
RTT_DIR = lib/RTT/RTT
RTT_CONFIG_DIR = lib/RTT/Config
RTT_SYS_DIR = lib/RTT/Syscalls
BUILD_DIR = build

# MCU specific flags for XMC1100 (Cortex-M0)
MCU_FLAGS = -mcpu=cortex-m0 -mthumb -DXMC1100_Q024x0064

# Linker script
LD_SCRIPT = $(XMCLIB_DIR)/CMSIS/Infineon/COMPONENT_XMC1100/Source/TOOLCHAIN_GCC_ARM/XMC1100x0064.ld

# Compiler and Linker flags
CFLAGS = $(MCU_FLAGS) -O2 -g3 -Wall -ffunction-sections -fdata-sections
LDFLAGS = $(MCU_FLAGS) -T$(LD_SCRIPT) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map -Wl,--gc-sections
LDFLAGS += --specs=nano.specs

# Include directories
INCLUDES = \
    -I$(INC_DIR) \
    -I$(XMCLIB_DIR)/XMCLib/inc \
    -I$(XMCLIB_DIR)/CMSIS/Infineon/COMPONENT_XMC1100/Include \
    -I$(CMSIS_CORE_DIR)/Include \
    -I$(RTT_DIR) \
    -I$(RTT_CONFIG_DIR) \
    -I$(RTT_SYS_DIR)

# Source files
USER_SRCS = $(wildcard $(SRC_DIR)/*.c)
HAL_SRCS = $(wildcard $(XMCLIB_DIR)/XMCLib/src/*.c)
RTT_SRCS = $(wildcard $(RTT_DIR)/*.c)
RTT_SYS_SRCS = $(wildcard $(RTT_SYS_DIR)/*.c)
SYS_SRCS = $(XMCLIB_DIR)/CMSIS/Infineon/COMPONENT_XMC1100/Source/system_XMC1100.c
ASM_SRCS = $(XMCLIB_DIR)/CMSIS/Infineon/COMPONENT_XMC1100/Source/TOOLCHAIN_GCC_ARM/startup_XMC1100.S

# Object files (mapping sources to build directory)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(USER_SRCS)) \
       $(patsubst %.c, $(BUILD_DIR)/%.o, $(HAL_SRCS)) \
       $(patsubst %.c, $(BUILD_DIR)/%.o, $(RTT_SRCS)) \
       $(patsubst %.c, $(BUILD_DIR)/%.o, $(RTT_SYS_SRCS)) \
       $(patsubst %.c, $(BUILD_DIR)/%.o, $(SYS_SRCS)) \
       $(patsubst %.S, $(BUILD_DIR)/%.o, $(ASM_SRCS))

# Default target
all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex
	$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

# Compile C source files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile ASM source files
$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link
$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Create binary
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

# Create hex
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

jlink:
	JLinkExe -device XMC1100-0064 -if SWD -speed 4000 -autoconnect 1

flash: all
	JLinkExe -device XMC1100-0064 -if SWD -speed 4000 -autoconnect 1 -CommanderScript flash.jlink

view:
	JLinkRTTClient

.PHONY: all clean flash