

INCLUDES  += -I$(LIB_DIR)libgloss-htif/include/
INCLUDES  += -I$(LIB_DIR)libgloss-htif/misc/

A_SOURCES += $(LIB_DIR)libgloss-htif/misc/crtmain.S

C_SOURCES += $(LIB_DIR)libgloss-htif/sys/sbrk.c
C_SOURCES += $(LIB_DIR)libgloss-htif/sys/write.c
C_SOURCES += $(LIB_DIR)libgloss-htif/misc/htif.c

