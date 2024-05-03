########################################################################################################################
# file:  Makefile
#
# Makefile for building the Baremetal-IDE firmware.
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#   Build instructions:
#     make build
########################################################################################################################

#################################
# Project Settings
#################################

# these variables can be overwritten by the command line

TARGET ?= firmware

CHIP ?= examplechip

# MCU Settings
ARCH ?= rv32imac
ABI ?= ilp32
CODEMODEL ?= medany

# Spec Settings
SPEC = nano.specs
# SPEC = lib/libgloss-htif/util/htif_nano.specs


#################################
# RISCV Toolchain
#################################

PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size


#################################
# Working directories
#################################

APP_DIR = app/
BSP_DIR = bsp/
DRIVER_DIR = driver/
LIB_DIR = lib/

SRC_DIR = $(APP_DIR)src/
INC_DIR = $(APP_DIR)inc/
BIN_DIR = dist/
BUILD_DIR = build/


#################################
# Source Files
#################################

### APP sources ###
INCLUDES   = -I$(INC_DIR)
A_SOURCES  = $(wildcard $(SRC_DIR)*.S) $(wildcard $(SRC_DIR)*/*.S)
C_SOURCES  = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)

### BSP sources ###
INCLUDES  += -I$(BSP_DIR)$(CHIP)
# A_SOURCES += $(BSP_DIR)$(CHIP)/boot/bootrom.S
A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S

-include $(BSP_DIR)$(CHIP)/Makefile

# ### DRIVER sources ###

# ### LIB sources ###
# INCLUDES  += $(foreach LIBRARY_NAME,$(LIBRARIES),-I$(LIB_DIR)$(LIBRARY_NAME)/inc)
# A_SOURCES += $(foreach LIBRARY_NAME,$(LIBRARIES),$(wildcard $(LIB_DIR)$(LIBRARY_NAME)/src/*.S))
# C_SOURCES += $(foreach LIBRARY_NAME,$(LIBRARIES),$(wildcard $(LIB_DIR)$(LIBRARY_NAME)/src/*.c))


#################################
# Object List
#################################

A_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(A_SOURCES))))
C_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(C_SOURCES))))

OBJECTS = $(A_OBJECTS) $(C_OBJECTS)


#################################
# Flags
#################################

# LD_SCRIPT ?= $(BSP_DIR)$(CHIP)/$(CHIP)_htif.ld
LD_SCRIPT ?= $(BSP_DIR)$(CHIP)/$(CHIP).ld

ifneq ($(CHIP),)
DEVICE_FLAGS  := -DCHIP=$(CHIP)
endif

# -mcmodel=medany -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc -static -nostartfiles -dT htif.ld
SPEC_FLAGS = --specs="$(SPEC)"

ARCH_FLAGS = -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie

# compiler Flags
CFLAGS  = -g -std=gnu11 -O0
CFLAGS += -fno-common -fno-builtin-printf
CFLAGS += -Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual
# CFLAGS += -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc
CFLAGS += $(SPEC_FLAGS)
CFLAGS += $(DEVICE_FLAGS)
CFLAGS += $(ARCH_FLAGS)
CFLAGS += $(INCLUDES)

# linker Flags
LFLAGS  = -static
LFLAGS += -nostartfiles
# LFLAGS += -nostdlib
LFLAGS += -u _printf_float
ifdef STACK_SIZE
LFLAGS += -Xlinker --defsym=__stack_size=$(STACK_SIZE)
endif
LFLAGS += -T $(LD_SCRIPT)


#################################
# Target Output Files
#################################

TARGET_ELF 		= $(BUILD_DIR)$(TARGET).elf
TARGET_BIN 		= $(BUILD_DIR)$(TARGET).bin
TARGET_HEX 		= $(BUILD_DIR)$(TARGET).hex
TARGET_VERILOG 	= $(BUILD_DIR)$(TARGET).out
TARGET_LST      = $(BUILD_DIR)$(TARGET).lst


#################################
# Internals
#################################

IDE_VERSION = 0.1.0

# read all supported chips from the bsp directory
SUPPORTED_CHIPS = $(sort $(shell ls -d $(BSP_DIR)*/ | xargs -n 1 basename | sed 's/\///g' | grep -v "scripts"))


#################################
# Build
#################################

.DEFAULT_GOAL := build

# default target
build: $(TARGET_ELF)
	@echo "[Build] $(TARGET_ELF) built for target \"$(CHIP)\""

$(TARGET_BIN): $(TARGET_ELF)
	$(CP) -O binary $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(CP) -O ihex $< $@

$(TARGET_VERILOG): $(TARGET_ELF)
	$(CP) -O verilog $< $@

$(TARGET_ELF): $(OBJECTS)
	@echo "[LD] linking $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
	$(SIZE) $(TARGET_ELF)

$(A_OBJECTS): $(BUILD_DIR)%.o: %.S
	@echo "[CC] compiling $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(C_OBJECTS): $(BUILD_DIR)%.o: %.c
	@echo "[CC] compiling $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@


#################################
# OpenOCD Upload Commands
#################################

UPLOAD_COMMANDS_SRAM  = -c "init"
UPLOAD_COMMANDS_SRAM += -c "load_image $(TARGET_ELF) 0x0"
UPLOAD_COMMANDS_SRAM += -c "reset"
# UPLOAD_COMMANDS_SRAM += -c "sleep 100"
# UPLOAD_COMMANDS_SRAM += -c "reg pc 0x08000000"
UPLOAD_COMMANDS_SRAM += -c "exit"

UPLOAD_COMMANDS_FLASH  = -c "exit"
UPLOAD_COMMANDS_FLASH += -c "program $(TARGET_BIN) 0x20000000"
UPLOAD_COMMANDS_FLASH += -c "reset"
UPLOAD_COMMANDS_FLASH += -c "exit"


#################################
# Recipes
#################################

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
	@rm -rf $(BUILD_DIR)

bin: $(TARGET_BIN)

hex: $(TARGET_HEX)

verilog: $(TARGET_VERILOG)

.PHONY: dump
dump:
	$(OD) -S -d $(TARGET_ELF) > $(TARGET_LST)
	$(OD) -d $(TARGET_ELF) > $(BUILD_DIR)disassemble.S
	$(OD) -h $(TARGET_ELF) > $(BUILD_DIR)sections.out
	$(OD) -t $(TARGET_ELF) > $(BUILD_DIR)symbol_table.out

# openocd currently only supports 32 bit target, thus we need to use binary loader
.PHONY: upload
upload: $(TARGET_BIN)
	@openocd -f ./debug/$(CHIP).cfg $(UPLOAD_COMMANDS_SRAM)
# @openocd -f ./debug/$(CHIP).cfg -c "init" -c "load_image ./build/firmware.elf 0x0" -c "reset" -c "sleep 100" -c "reg pc 0x08000000" -c "exit"
# @openocd -f ./debug/$(CHIP).cfg -c "program $(TARGET_BIN) 0x20000000 reset exit"

.PHONY: debug
debug: $(TARGET_BIN)
	@openocd -f ./debug/$(CHIP).cfg & $(DG) --eval-command="target extended-remote localhost:3333"

## version           : print firmware version
version:
	@echo "Baremetal-IDE Version $(IDE_VERSION)"

## help              : print this help message and exit
help:
	@echo "Makefile for Baremetal-IDE"
	@echo ""
	@echo "Usage:"
	@echo "    make [TARGET=<target_name>] [CHIP=<chipname>]"
	@echo ""
	@echo "To clean the project, run:"
	@echo "    make clean"
	@echo ""
	@echo "Valid CHIP values are: $(SUPPORTED_CHIPS)"
	@echo ""

