#ifndef __FESVR_H
#define __FESVR_H

/* Frontend system calls supported by fesvr */
#define FESVR_getcwd 17
#define FESVR_fcntl 25
#define FESVR_mkdirat 34
#define FESVR_unlinkat 35
#define FESVR_linkat 37
#define FESVR_renameat 38
#define FESVR_ftruncate 46
#define FESVR_faccessat 48
#define FESVR_chdir 49
#define FESVR_openat 56
#define FESVR_close 57
#define FESVR_lseek 62
#define FESVR_read 63
#define FESVR_write 64
#define FESVR_pread 67
#define FESVR_pwrite 68
#define FESVR_fstatat 79
#define FESVR_fstat 80
#define FESVR_exit 93
#define FESVR_lstat 1039
#define FESVR_getmainvars 2011

#endif /* __FESVR_H */
