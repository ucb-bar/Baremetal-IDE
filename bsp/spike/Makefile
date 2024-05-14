
ARCH 		?= rv64imafdc
ABI 		?= lp64d
CODEMODEL 	?= medany

# Makefile config equivalent with htif_nano.specs:
# -mcmodel=medany -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc -static -nostartfiles -dT htif.ld

A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S

include $(DRIVER_DIR)rv/Makefile
include $(DRIVER_DIR)htif/Makefile
