#include "mte.h"
#include "mte_irt.h"

static int try_overflow(uint64_t amt) {
    uint64_t rd;

    for (uint64_t tag_i = 0; tag_i < 0x10; tag_i++) {
        uint64_t rs1 = support_mte_tag_insert(-1, tag_i);
        uint64_t rs2 = amt;
        uint64_t expected = support_mte_tag_insert(rs1 + rs2, tag_i);
        // printf("rs1 = %lx, rs2 = %lx, expected = %lx\n", rs1, rs2, expected);
        MTE_ADD(5, rd, 6, rs1, 7, rs2);
        if (rd != expected) {
            printf("fail: tag_i = %lx, rd = %lx, expected = %lx\n", tag_i, rd, expected);
            return 1;
        }
    }

    return 0;
}

int run_mte_irt(void) {
    for (int i = 0; i < 25; i++) {
        uint64_t irt_rd = 0, irt_rs1 = 0, irt_rs2 = 0;
        while (!(irt_rd >> TAG_SHIFT)) {
            irt_rs1 = (uint64_t)DRAM_BASE;
            irt_rs2 = 0xAAAA<<5 | (uint64_t)i;
            MTE_IRT(5, irt_rd, 6, irt_rs1, 7, irt_rs2);
            printf("%i: %lx\n", i, irt_rd);
        }

    }

    printf("Test pass!\n");
    return 0;
}
