#include "mte.h"
#include "mte_add.h"

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

int run_mte_add() {
    if (try_overflow(1) || try_overflow(0xfefefefefe) || try_overflow(-1)) {
        printf("Test failed!\n");
        return 1;
    }

    printf("Test pass!\n");
    return 0;
}
