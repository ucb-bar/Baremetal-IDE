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
TARGET 		?= app
CHIP 		?= spike


#################################
# RISCV Toolchain
#################################

PREFIX 	:= riscv64-unknown-elf-

CC 		:= $(PREFIX)gcc
CXX 	:= $(PREFIX)g++
CP 		:= $(PREFIX)objcopy
OD 		:= $(PREFIX)objdump
DG 		:= $(PREFIX)gdb
SIZE 	:= $(PREFIX)size


#################################
# Working directories
#################################

ROOT_DIR	?= ./

APP_DIR 	?= $(ROOT_DIR)$(TARGET)/
BSP_DIR  	:= $(ROOT_DIR)bsp/
DRIVER_DIR 	:= $(ROOT_DIR)driver/
LIB_DIR 	:= $(ROOT_DIR)lib/

BUILD_DIR 	:= $(ROOT_DIR)build/
BIN_DIR 	:= $(ROOT_DIR)dist/


#################################
# Chip Files
#################################

-include $(BSP_DIR)$(CHIP)/Makefile

# MCU Settings
ARCH 		?= rv64imafdc
ABI 		?= lp64d
CODEMODEL 	?= medany

# Spec Settings
SPEC 		?= nosys.specs

# Linker Script
LD_SCRIPT 	?= $(BSP_DIR)$(CHIP)/$(CHIP).ld


#################################
# Source Files
#################################

### APP sources ###
APP_INC_DIR := $(APP_DIR)inc/
APP_SRC_DIR := $(APP_DIR)src/

INCLUDES  += -I$(APP_INC_DIR)
A_SOURCES += $(wildcard $(APP_SRC_DIR)*.S) $(wildcard $(APP_SRC_DIR)*/*.S)
C_SOURCES += $(wildcard $(APP_SRC_DIR)*.c) $(wildcard $(APP_SRC_DIR)*/*.c)

### BSP sources ###
INCLUDES  += -I$(BSP_DIR)$(CHIP)

# ### DRIVER sources ###

# ### LIB sources ###
# INCLUDES  += $(foreach LIBRARY_NAME,$(LIBRARIES),-I$(LIB_DIR)$(LIBRARY_NAME)/inc)
# A_SOURCES += $(foreach LIBRARY_NAME,$(LIBRARIES),$(wildcard $(LIB_DIR)$(LIBRARY_NAME)/src/*.S))
# C_SOURCES += $(foreach LIBRARY_NAME,$(LIBRARIES),$(wildcard $(LIB_DIR)$(LIBRARY_NAME)/src/*.c))

# include project-specific dependencies
-include $(TARGET)/Makefile


#################################
# Object List
#################################

A_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(A_SOURCES))))
C_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(C_SOURCES))))

OBJECTS = $(A_OBJECTS) $(C_OBJECTS)


#################################
# Flags
#################################

OPTIMIZATION_FLAGS := -O0
F_FLAGS := -ffunction-sections -fdata-sections -fno-common -fno-builtin-printf
WARNING_FLAGS := -Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual
ARCH_FLAGS := -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie
SPEC_FLAGS := --specs="$(SPEC)"

ifneq ($(CHIP),)
CHIP_FLAGS += -DCHIP=$(CHIP)
endif

# compiler Flags
CFLAGS := -g -std=gnu11
CFLAGS += $(OPTIMIZATION_FLAGS)
CFLAGS += $(F_FLAGS)
CFLAGS += $(WARNING_FLAGS)
CFLAGS += $(ARCH_FLAGS)
CFLAGS += $(SPEC_FLAGS)
CFLAGS += $(CHIP_FLAGS)
CFLAGS += $(INCLUDES)

# linker Flags
LFLAGS := -static
LFLAGS += -nostartfiles
# LFLAGS += -u _printf_float
ifdef STACK_SIZE
LFLAGS += -Xlinker --defsym=__stack_size=$(STACK_SIZE)
endif
# LFLAGS += -lm
LFLAGS += -T $(LD_SCRIPT)


#################################
# Target Output Files
#################################

TARGET_NAME ?= $(shell basename $(TARGET))

TARGET_ELF 		:= $(BUILD_DIR)$(TARGET_NAME).elf
TARGET_BIN 		:= $(BUILD_DIR)$(TARGET_NAME).bin
TARGET_HEX 		:= $(BUILD_DIR)$(TARGET_NAME).hex
TARGET_VERILOG 	:= $(BUILD_DIR)$(TARGET_NAME).out
TARGET_LST      := $(BUILD_DIR)$(TARGET_NAME).lst


#################################
# Internals
#################################

IDE_VERSION 	:= 0.1.0

# read all supported chips from the bsp directory
SUPPORTED_CHIPS := $(sort $(shell ls -d $(BSP_DIR)*/ | xargs -n 1 basename | sed 's/\///g' | grep -v "scripts"))


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
	@$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)
	@$(SIZE) $(TARGET_ELF)

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
	@echo "    make [TARGET=<path_to_project>] [CHIP=<chipname>]"
	@echo ""
	@echo "To clean the project, run:"
	@echo "    make clean"
	@echo ""
	@echo "Valid CHIP values are: $(SUPPORTED_CHIPS)"
	@echo ""

test:
	@echo $(shell basename $(TARGET))
	@echo $(C_SOURCES)
	@echo $(APP_SRC_DIR)

create_project:
	python3 bsp/scripts/create_project.py $(chip)