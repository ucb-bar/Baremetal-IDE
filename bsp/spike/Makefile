
ARCH 		?= rv64imafdc
ABI 		?= lp64d
CODEMODEL 	?= medany

# equivalent with htif_nano.specs:
# -mcmodel=medany -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc -static -nostartfiles -dT htif.ld

# CHIP_FLAGS  := -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc
# CHIP_FLAGS  := -Wl,--start-group -lc_nano -Wl,--end-group -lgcc


A_SOURCES += $(BSP_DIR)$(CHIP)/boot/startup.S

include $(DRIVER_DIR)rv/Makefile
# include $(DRIVER_DIR)rv-sys/Makefile

include $(DRIVER_DIR)htif/Makefile
