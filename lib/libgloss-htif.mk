

INCLUDES  += -I$(LIB_DIR)libgloss-htif/include/
INCLUDES  += -I$(LIB_DIR)libgloss-htif/misc/

A_SOURCES += $(LIB_DIR)libgloss-htif/misc/crt0.S
A_SOURCES += $(LIB_DIR)libgloss-htif/misc/init.S
A_SOURCES += $(LIB_DIR)libgloss-htif/misc/crtmain.S

C_SOURCES += $(LIB_DIR)libgloss-htif/misc/htif.c
C_SOURCES += $(LIB_DIR)libgloss-htif/misc/main.c
C_SOURCES += $(LIB_DIR)libgloss-htif/misc/tls.c
C_SOURCES += $(LIB_DIR)libgloss-htif/misc/traps.c

C_SOURCES += $(LIB_DIR)libgloss-htif/sys/sbrk.c
C_SOURCES += $(LIB_DIR)libgloss-htif/sys/write.c

C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/printbuf.c
C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/printf.c
C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/puts.c
C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/snprintf.c
C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/sprintf.c
C_SOURCES += $(LIB_DIR)libgloss-htif/libc/stdio/vxprintf.c
