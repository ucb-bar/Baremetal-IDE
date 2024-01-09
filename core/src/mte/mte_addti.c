#include "mte.h"
#include "mte_addti.h"

static uintptr_t 
sw_mte_addti(uintptr_t ptr, int8_t ptr_offset, uint8_t tag_off, uint8_t permissive_tag) {
    uint8_t tag = support_mte_tag_extract(ptr);
    
    ptr += ptr_offset;

    tag += tag_off;
    tag &= 0xf;
    if (tag == permissive_tag) {
        tag += 1;
        tag &= 0xf;
    }

    ptr = support_mte_tag_insert(ptr, tag);
    return ptr;
}

static int
test_x(uint8_t permissive_tag, uintptr_t addr) {
    uint64_t mte_config = 
        (permissive_tag << CSR_MTE_PERMISSIVE_TAG_SHIFT);
    CSRRW(CSR_MTE_CONFIG, mte_config);

    uintptr_t sw_addr;
    uint64_t rd = 0, rs1 = addr;
    #define TEST_CASE(ADDR_OFF, TAG_OFF) { \
        MTE_ADDTI(5, rd, 6, rs1, ADDR_OFF, TAG_OFF); \
        sw_addr = sw_mte_addti(addr, ADDR_OFF, TAG_OFF, permissive_tag); \
        if (sw_addr != rd) { \
            printf("fail addr=%lx, addr_off=%d, tag_off=%x, permissive_tag=%x, %lx!=%lx\n", addr, ADDR_OFF, TAG_OFF, permissive_tag, rd, sw_addr); \
            return 1; \
        } \
    }

    #define TEST_CASE1(ADDR_OFF) \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 0); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 1); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 2); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 3); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 4); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 5); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 6); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 7); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 8); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 9); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 10); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 11); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 12); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 13); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 14); \
        TEST_CASE(ADDR_OFF, /* tag_off*/ 15);

    TEST_CASE1(/* addr_off */ 0);
    TEST_CASE1(/* addr_off */ 1);
    TEST_CASE1(/* addr_off */ -1);
    TEST_CASE1(/* addr_off */ 128);
    TEST_CASE1(/* addr_off */ -127);

    #undef TEST_CASE
    #undef TEST_CASE1

    return 0;
}

int run_mte_addti() {
    for (uint8_t tag = 0; tag <= 0xf; tag++) {
        if (test_x(tag, 1) || test_x(tag, 0xfefefefefe) || test_x(tag, -1)) {
            return 1;
        }
    }

    printf("Test pass!\n");
    return 0;
}
