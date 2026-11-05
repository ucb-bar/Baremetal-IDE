/*
 * saturn_compat.h - what the Saturn benchmarks (generators/saturn/benchmarks) use from
 * riscv-tests' common/syscalls.c that glossy/newlib don't provide.
 *
 * The benchmarks only declare setStats() themselves (common/util.h). printstr/printhex are
 * provided for completeness; the rest of syscalls.c (printf, memcpy, exit, ...) is covered by
 * newlib + glossy. saturn_compat.c also overrides glossy's trap_handler (any trap -> exit 1337).
 */
#ifndef __SATURN_COMPAT_H
#define __SATURN_COMPAT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* enable=1 snapshots mcycle/minstret; enable=0 prints the deltas as
 * "mcycle = N" / "minstret = N" (syscalls.c printed the same lines after main returned). */
void setStats(int enable);
void printstr(const char *s);
void printhex(uint64_t x);

#ifdef __cplusplus
}
#endif

#endif /* __SATURN_COMPAT_H */
