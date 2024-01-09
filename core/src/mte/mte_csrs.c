#include "mte.h"

int run_mte_csrs(void) {
    uint64_t conf = 0;
    CSRRR(CSR_MTE_FA, conf);
    printf("initial read: %lx\n", conf);
    printf("putting 1\n");
    conf = 1;
    CSRRW(CSR_MTE_FA, conf);
    printf("reading...\n");
    CSRRR(CSR_MTE_FA, conf);
    printf("got %lx\n", conf);
    return 0;
}
