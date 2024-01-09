#include "mte.h"
#include "mte_dbg.h"

#define TEST_LG2_SETS 3
#define TEST_LG2_OFF 3
#define WAY_COUNT 4
static inline uint64_t
dbg_req_gen(uint64_t type, 
            uint64_t set, uint64_t way) {
    return type
            | set << (2)
            | way << (2 + TEST_LG2_SETS);
}

void 
dbg_command(uint64_t req, uint64_t *data0, uint64_t *data1) {
    uint64_t read_req;
    CSRRW(CSR_MTE_DBG_TCACHE_DATA0, *data0);
    CSRRW(CSR_MTE_DBG_TCACHE_DATA1, *data1);
    CSRRW(CSR_MTE_DBG_TCACHE_REQ, req);
    /* wait for completion */    
    do {
        CSRRR(CSR_MTE_DBG_TCACHE_REQ, read_req)
    } while (read_req);

    uint64_t read_data0, read_data1;
    CSRRR(CSR_MTE_DBG_TCACHE_DATA0, read_data0);
    CSRRR(CSR_MTE_DBG_TCACHE_DATA1, read_data1);
    *data0 = read_data0;
    *data1 = read_data1;
}

void
dbg_read_raw(uint64_t set, uint64_t way, uint64_t *meta, uint64_t *data) {
    dbg_command(dbg_req_gen(DBG_TCACHE_REQ_TYPE_RAW_READ, set, way), data, meta);
}

void
dbg_write_raw(uint64_t set, uint64_t way, uint64_t meta, uint64_t data) {
    dbg_command(dbg_req_gen(DBG_TCACHE_REQ_TYPE_RAW_WRITE, set, way), &data, &meta);
}

void 
dbg_flush(void) {
    uint64_t data = 0, meta = 0;
    dbg_command(DBG_TCACHE_REQ_TYPE_FLUSH, &data, &meta);
}

static uintptr_t phys_to_tag_addr(uintptr_t phys) {
    uintptr_t tag_byte_idx = (phys - DRAM_BASE) >> 5;
    return DRAM_TAG_STORE + tag_byte_idx;
}

int run_mte_dbg(void) {
    printf("BEGIN\n");

    uint64_t dram_tag_base = DRAM_TAG_STORE;
    CSRRW(CSR_MTE_TAG_BASE0, dram_tag_base);
    uint64_t dram_tag_mask = DRAM_TAG_MASK;
    CSRRW(CSR_MTE_TAG_MASK0, dram_tag_mask);
    uint64_t mte_config = 
        /* Run with tagging off to avoid tag fetch noise */
        (0x0 << CSR_MTE_CONFIG_ENABLE_SHIFT) |
        (0x0 << CSR_MTE_PERMISSIVE_TAG_SHIFT);
    CSRRW(CSR_MTE_CONFIG, mte_config);
    CSRRR(CSR_MTE_CONFIG, mte_config);

    uint64_t arr[2];
    uint64_t target_addr = phys_to_tag_addr((uintptr_t)&arr);
    uint64_t target_set = (target_addr >> TEST_LG2_OFF) & ((1 << TEST_LG2_SETS) - 1);
    uint64_t target_tag = (target_addr >> (TEST_LG2_OFF + TEST_LG2_SETS));

    /* Put a marker in the target addr dword so we can check for flush */
    volatile uint64_t *target_line = (volatile uint64_t *)(target_addr & (~0x7UL));
    uint64_t target_line_no_writeback = 0xaaaaaaaaaaaaaaaa;
    uint64_t target_line_writeback    = 0xbbbbbbbbbbbbbbbb;
    *target_line = target_line_no_writeback;

    /* Forge a clean entry for our target */
    uint64_t meta = 
        1 << CSR_MTE_DBG_TCACHE_META_VALID_SHIFT |
        0 << CSR_MTE_DBG_TCACHE_META_DIRTY_SHIFT |
        target_tag << CSR_MTE_DBG_TCACHE_META_TAG_SHIFT;
    uint64_t data = target_line_writeback;

    dbg_write_raw(target_set, /* way */ 0, meta, data);

    if (*target_line != target_line_no_writeback) {
        printf("fail 0: unexpected writeback? %lx\n", *target_line);
    }

    uint64_t rr_data = 0, rr_meta = 0;
    dbg_read_raw(target_set, /* way */ 0, &rr_meta, &rr_data);
    if (rr_meta != meta || rr_data != data) {
        printf("fail 1: re-read mismatched. Meta (%lx -> %lx) Data (%lx -> %lx)\n", meta, rr_meta, data, rr_data);
    }

    /* Trigger a flush. The line is clean so there should be no wb */
    dbg_flush();
    if (*target_line != target_line_no_writeback) {
        printf("fail 2: unexpected writeback? %lx\n", *target_line);
    }

    /* Now try dirtying the line */
    meta |= (1 << CSR_MTE_DBG_TCACHE_META_DIRTY_SHIFT);
    dbg_write_raw(target_set, /* way */ 0, meta, data);

    if (*target_line != target_line_no_writeback) {
        printf("fail 3: unexpected writeback? %lx\n", *target_line);
    }

    dbg_read_raw(target_set, /* way */ 0, &rr_meta, &rr_data);
    if (rr_meta != meta || rr_data != data) {
        printf("fail 4: re-read mismatched. Meta (%lx -> %lx) Data (%lx -> %lx)\n", meta, rr_meta, data, rr_data);
    }

    /* Now flush with dirty data */
    dbg_flush();
    if (*target_line != target_line_writeback) {
        printf("fail 5: expected writeback? %lx\n", *target_line);
    }

    printf("OK\n");

    return 0;
}
