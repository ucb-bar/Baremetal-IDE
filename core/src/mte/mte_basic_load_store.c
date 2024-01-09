#include "mte.h"
#include "mte_basic_load_store.h"
#include "main.h"

int run_mte_basic_load_store(void) {
    printf("hello world\n");
    // for (uintptr_t clr_i = DRAM_TAG_STORE; clr_i < DRAM_TAG_STORE + DRAM_TAG_MASK; clr_i += 32) {
    //     *(volatile uint64_t *)(clr_i + 0) = 0xffffffffffffffffULL;
    //     *(volatile uint64_t *)(clr_i + 8) = 0xffffffffffffffffULL;
    //     *(volatile uint64_t *)(clr_i + 16) = 0xffffffffffffffffULL;
    //     *(volatile uint64_t *)(clr_i + 24) = 0xffffffffffffffffULL;
    // }
    uint64_t dram_tag_base = DRAM_TAG_STORE;
    CSRRW(CSR_MTE_TAG_BASE0, dram_tag_base);
    uint64_t dram_tag_mask = DRAM_TAG_MASK;
    CSRRW(CSR_MTE_TAG_MASK0, dram_tag_mask);
    uint64_t mte_config = 
        (0b1111 << CSR_MTE_CONFIG_ENABLE_SHIFT) |
        (0xf << CSR_MTE_PERMISSIVE_TAG_SHIFT);
    CSRRW(CSR_MTE_CONFIG, mte_config);
    CSRRR(CSR_MTE_CONFIG, mte_config);
    printf("CSR_MTE_CONFIG=0x%lx\n",mte_config);

    asm volatile ("rdcycle x0");

    // for (uintptr_t clr_i = DRAM_BASE; clr_i < dram_tag_base; clr_i += 64) {
    //     uint64_t stti_rd = 0;
    //     uint64_t stti_rs1 = support_mte_tag_insert(clr_i, 0xf);
    //     /* 0x8000ABC0 := B */
    //     MTE_STTI(stti_rd, stti_rs1, /* rd */ 5, /* rs1 */ 6, 0b000000000000);
    //     MTE_STTI(stti_rd, stti_rs1, /* rd */ 5, /* rs1 */ 6, 0b000000010000);
    //     MTE_STTI(stti_rd, stti_rs1, /* rd */ 5, /* rs1 */ 6, 0b000000100000);
    //     MTE_STTI(stti_rd, stti_rs1, /* rd */ 5, /* rs1 */ 6, 0b000000110000);
    // }

    // for (uintptr_t clr_i = DRAM_BASE; clr_i < dram_tag_base; clr_i += 512) {
    //     for (uintptr_t clr_ii = 0; clr_ii < 256; clr_ii += 16) {
    //         for (uintptr_t clr_iii = 0; clr_iii < 2; clr_iii++) {
    //             uint64_t stti_rd = 0;
    //             uint64_t stti_rs1 = support_mte_tag_insert(clr_i + clr_ii + 256 * clr_iii, 0xf);
    //             MTE_STTI(stti_rd, stti_rs1, /* rd */ 0, /* rs1 */ 6, 0b000000000000);
    //         }
    //     }
    // }


    uint64_t fstatus = 0;
    CSRRR(CSR_MTE_FSTATUS, fstatus);
    printf("fstatus1 = %lx\n", fstatus);
    
    volatile uint8_t *dram = (uint8_t *)support_mte_tag_insert(DRAM_BASE + 0xabc0, 0xa);
    printf("dram ptr = %p\n", dram);
    *dram = 0xabcdef;
    printf("dram = %lx\n", *dram);
    
    fstatus = 0;
    CSRRW(CSR_MTE_FSTATUS, fstatus);

    uint64_t stti_rs1 = dram;
    /* 0x8000ABC0 := B */
    MTE_STTI(stti_rs1, /* rs1 */ 6, /* addr off<<4 */ 0b00000000, /* tag off */ 0b0001);
    /* 0x8000ABD0 := C */
    MTE_STTI(stti_rs1, /* rs1 */ 6, /* addr off<<4 */ 0b00000001, /* tag off */ 0b0010);
    /* 0x8000ABE0 := D */
    MTE_STTI(stti_rs1, /* rs1 */ 6, /* addr off<<4 */ 0b00000010, /* tag off */ 0b0011);
    /* 0x8000ABB0 := A */
    MTE_STTI(stti_rs1, /* rs1 */ 6, /* addr off<<4 */ 0b11111111, /* tag off */ 0b0000);

    *((volatile uint8_t *)(support_mte_tag_insert((uint64_t)dram, 0xB)));
    *((volatile uint8_t *)(support_mte_tag_insert((uint64_t)dram + 16, 0xC)));
    *((volatile uint8_t *)(support_mte_tag_insert((uint64_t)dram + 32, 0xD)));
    *((volatile uint8_t *)(support_mte_tag_insert((uint64_t)dram - 16, 0xA)));

    CSRRR(CSR_MTE_FSTATUS, fstatus);
    uint64_t fa = 0;
    CSRRR(CSR_MTE_FA, fa);
    printf("fstatus2 = %lx, fa = 0x%lx\n", fstatus, fa);

    fstatus = 0;
    CSRRW(CSR_MTE_FSTATUS, fstatus);
    *((volatile uint8_t *)(support_mte_tag_insert((uint64_t)dram, 0xC)));

    CSRRR(CSR_MTE_FSTATUS, fstatus);
    CSRRR(CSR_MTE_FA, fa);
    printf("fstatus3 = %lx, fa = 0x%lx\n", fstatus, fa);

    // asm volatile ("rdcycle x0");
    // printf("dram 2 = %lx\n", *dram);
    // printf("dram 3 = %lx\n", *(dram + 16));
    // printf("dram 4 = %lx\n", *(dram + 32));

    // printf("tag mem 008f000558=%lx\n", *(volatile uint64_t *)(0x008f000558));
    return 0;
}

