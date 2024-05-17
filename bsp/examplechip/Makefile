
ARCH 		= rv64imac
ABI 		= lp64
CODEMODEL 	= medany

INCLUDES  += -I$(BSP_DIR)$(CHIP)

A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S
C_SOURCES += $(BSP_DIR)$(CHIP)/examplechip.c

include $(DRIVER_DIR)rv/Makefile

include $(DRIVER_DIR)rocket-chip/Makefile
include $(DRIVER_DIR)rocket-chip-blocks/Makefile

include $(LIB_DIR)syscall/Makefile
