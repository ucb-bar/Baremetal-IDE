
ARCH 		?= rv64imafdc
ABI 		?= lp64d
CODEMODEL 	?= medany

# Makefile config equivalent with htif_nano.specs:
# -mcmodel=medany -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc -static -nostartfiles -dT htif.ld
INCLUDES  += -I$(BSP_DIR)$(CHIP)

A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S
C_SOURCES += $(BSP_DIR)$(CHIP)/qemuvirt.c

include $(DRIVER_DIR)rv/Makefile

include $(DRIVER_DIR)national-semiconductor/Makefile
include $(DRIVER_DIR)syscon/Makefile
include $(DRIVER_DIR)rocket-chip/Makefile

include $(LIB_DIR)syscall/Makefile