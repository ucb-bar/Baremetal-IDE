
ARCH 		= rv64imafdc
ABI 		= lp64d
CODEMODEL 	= medany

INCLUDES  += -I$(BSP_DIR)$(CHIP)

A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S
C_SOURCES += $(BSP_DIR)$(CHIP)/eaglex_prci.c

include $(DRIVER_DIR)rv/Makefile

include $(DRIVER_DIR)rocket-chip/Makefile
include $(DRIVER_DIR)rocket-chip-blocks/Makefile

include $(LIB_DIR)syscall/Makefile
