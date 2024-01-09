#ifndef __MTE__
#define __MTE__

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#define TAG_SHIFT (60)
#define TAG_MASK (15LU << TAG_SHIFT)

static inline uint64_t 
support_mte_tag_insert(uint64_t ptr, uint64_t tag) {
    return (ptr & ~TAG_MASK) | (tag << TAG_SHIFT);
}

static inline uint64_t
support_mte_tag_clear(uint64_t ptr) {
    return support_mte_tag_insert(ptr, 0);
}

static inline uint64_t
support_mte_tag_extract(uint64_t ptr) {
    return ptr >> TAG_SHIFT;
}

#define STR1(x) #x
#define STR(x) STR1(x)

#define CUSTOM_3 0b1111011
#define MTE_OPCODE CUSTOM_3
#define MTE_OPCODE_R_TYPE_FUNCT_3 7

#define CUSTOM_R_TYPE(opcode, rd, funct3, rs1, rs2, funct7)  \
    opcode | \
    ((rd) << 7) | \
    ((funct3) << 12) | \
    ((rs1) << 15) | \
    ((rs2) << 20) | \
    ((funct7 << 25))

#define MTE_R_TYPE(rd, rs1, rs2, funct7) \
    CUSTOM_R_TYPE(MTE_OPCODE, rd, MTE_OPCODE_R_TYPE_FUNCT_3, rs1, rs2, funct7)

#define MTE_R_TYPE_INSTRUCTION(rd_v, rs1_v, rs2_v, funct7, rd_n, rs1_n, rs2_n) { \
    register uint64_t rd_  asm ("x" # rd_n);                                 \
    register uint64_t rs1_ asm ("x" # rs1_n) = (uint64_t) rs1_v;               \
    register uint64_t rs2_ asm ("x" # rs2_n) = (uint64_t) rs2_v;               \
    asm volatile (                                                           \
        ".word " STR(MTE_R_TYPE(rd_n, rs1_n, rs2_n, funct7)) "\n\t"  \
        : "=r" (rd_)                                                         \
        : [_rs1] "r" (rs1_), [_rs2] "r" (rs2_)); \
    rd_v = rd_;                                                                \
  }
  
#define MTE_ADD(rd_n, rd_v, rs1_n, rs1_v, rs2_n, rs2_v) \
    MTE_R_TYPE_INSTRUCTION(rd_v, rs1_v, rs2_v, 0, rd_n, rs1_n, rs2_n)

#define MTE_IRT(rd_n, rd_v, rs1_n, rs1_v, rs2_n, rs2_v) \
    MTE_R_TYPE_INSTRUCTION(rd_v, rs1_v, rs2_v, 1, rd_n, rs1_n, rs2_n)


#define CUSTOM_I_TYPE(opcode, rd, funct3, rs1, imm12)  \
    opcode | \
    ((rd) << 7) | \
    ((funct3) << 12) | \
    ((rs1) << 15) | \
    ((imm12) << 20)

#define MTE_I_TYPE(rd, funct3, rs1, imm12) \
    CUSTOM_I_TYPE(MTE_OPCODE, rd, funct3, rs1, imm12)

#define MTE_I_TYPE_INSTRUCTION(rd_v, rs1_v, funct3, rd_n, rs1_n, imm12) { \
    register uint64_t rd_  asm ("x" # rd_n);                                 \
    register uint64_t rs1_ asm ("x" # rs1_n) = (uint64_t) rs1_v;               \
    asm volatile (                                                           \
        ".word " STR(MTE_I_TYPE(rd_n, funct3, rs1_n, imm12)) "\n\t"  \
        : "=r" (rd_)                                                         \
        : [_rs1] "r" (rs1_)); \
    rd_v = rd_;                                                                \
  }

#define MTE_ADDTI_FUNCT3 6
#define MTE_ADDTI(rd_n, rd_v, rs1_n, rs1_v, addr_off_imm, tag_off_imm) \
    MTE_I_TYPE_INSTRUCTION(rd_v, rs1_v, \
        MTE_ADDTI_FUNCT3, \
        rd_n, rs1_n, \
        (addr_off_imm) << 4 | tag_off_imm \
    )
  

#define CUSTOM_S_TYPE(opcode, funct3, rs1, rs2, imm12)  \
    opcode | \
    ((imm12 & 0x1f) << 7) | \
    ((funct3) << 12) | \
    ((rs1) << 15) | \
    ((rs2) << 20) | \
    (((imm12) >> 5) << 25)

#define MTE_S_TYPE(funct3, rs1, rs2, imm12) \
    CUSTOM_S_TYPE(MTE_OPCODE, funct3, rs1, rs2, imm12)

#define MTE_S_TYPE_INSTRUCTION(rs1_v, rs2_v, funct3, rs1_n, rs2_n, imm12) { \
    register uint64_t rs1_ asm ("x" # rs1_n) = (uint64_t) rs1_v;               \
    register uint64_t rs2_ asm ("x" # rs2_n) = (uint64_t) rs2_v;               \
    asm volatile (                                                           \
        ".word " STR(MTE_S_TYPE(MTE_STTI_FUNCT3, rs1_n, rs2_n, imm12)) "\n\t"  \
        :                                                         \
        : [_rs1] "r" (rs1_), [_rs2] "r" (rs2_)); \
  }


#define MTE_STTI_FUNCT3 5
#define MTE_STTI(rs1_v, rs1_n, addr_off_imm, tag_off_imm) \
    MTE_S_TYPE_INSTRUCTION(rs1_v, 0, \
        MTE_STTI_FUNCT3, \
        rs1_n, 0, \
        (addr_off_imm) << 4 | tag_off_imm \
    )

/*
                                    ~* CSRs *~
*/
#define CSRRW(reg, argl) \
    asm volatile (                                                           \
        "csrw " STR(reg) ", %0\n\t"  \
        :                                                                    \
        : "r" (argl)); 

#define CSRRR(reg, argl) \
    asm volatile (                                                           \
        "csrr %0, " STR(reg) "\n\t"  \
        : "=r" (argl)                                                                   \
        : ); 

#define CSR_MTE_CONFIG 0x5c0
#define CSR_MTE_CONFIG_ENABLE_SHIFT 0
#define CSR_MTE_CONFIG_ENABLE_MASK 0b1111
#define CSR_MTE_CONFIG_ENFORCE_SYNC_SHIFT 4
#define CSR_MTE_CONFIG_ENFORCE_SYNC_MASK 0b1111
#define CSR_MTE_PERMISSIVE_TAG_SHIFT 8
#define CSR_MTE_PERMISSIVE_TAG_MASK 0b1111

#define CSR_MTE_FSTATUS 0x5c1
/*
    def smte_fstatus_validShift         =  0
    def smte_fstatus_validWidth         =  1
    def smte_fstatus_addressTagShift = smte_fstatus_validShift + smte_fstatus_validWidth
    def smte_fstatus_addressTagWidth = MTEConfig.tagBits
    def smte_fstatus_physicalTagShift =  smte_fstatus_addressTagShift + smte_fstatus_addressTagWidth
    def smte_fstatus_physicalTagWidth =  MTEConfig.tagBits
    def smte_fstatus_privShift = smte_fstatus_physicalTagShift + smte_fstatus_physicalTagWidth
    def smte_fstatus_privWidth = 2
    def smte_fstatus_isLoadShift = smte_fstatus_privShift + smte_fstatus_privWidth
    def smte_fstatus_isLoadWidth = 1
    def smte_fstatus_opSizeShift = smte_fstatus_isLoadShift + smte_fstatus_isLoadWidth
    def smte_fstatus_opSizeShift = 2
*/

#define CSR_MTE_FSTATUS_VALID_SHIFT 0
#define CSR_MTE_FSTATUS_VALID_MASK 0b1
#define CSR_MTE_FSTATUS_ADDRESS_TAG_SHIFT 1
#define CSR_MTE_FSTATUS_ADDRESS_TAG_MASK 0b1111
#define CSR_MTE_FSTATUS_PHYSICAL_TAG_SHIFT 5
#define CSR_MTE_FSTATUS_PHYSICAL_TAG_MASK 0b1111
#define CSR_MTE_FSTATUS_PRV_SHIFT 9
#define CSR_MTE_FSTATUS_PRV_MASK 0b11
#define CSR_MTE_FSTATUS_IS_LOAD_SHIFT 11
#define CSR_MTE_FSTATUS_IS_LOAD_MASK 0b1
#define CSR_MTE_FSTATUS_OP_SIZE_SHIFT 12
#define CSR_MTE_FSTATUS_OP_SIZE_MASK 0b11



#define CSR_MTE_FA 0x5c2
#define CSR_MTE_TAG_BASE0 0x5c3
#define CSR_MTE_TAG_MASK0 0x5c4
#define CSR_MTE_TAG_BASE1 0x5c5
#define CSR_MTE_TAG_MASK1 0x5c6
#define CSR_MTE_TAG_BASE2 0x5c7
#define CSR_MTE_TAG_MASK2 0x5c8

#define CSR_MTE_DBG_TCACHE_REQ 0x5cd
#define CSR_MTE_DBG_TCACHE_DATA0 0x5ce
#define CSR_MTE_DBG_TCACHE_DATA1 0x5cf
#define CSR_MTE_DBG_TCACHE_META_VALID_SHIFT 0
#define CSR_MTE_DBG_TCACHE_META_VALID_MASK 1
#define CSR_MTE_DBG_TCACHE_META_DIRTY_SHIFT 1
#define CSR_MTE_DBG_TCACHE_META_DIRTY_MASK 1
#define CSR_MTE_DBG_TCACHE_META_TAG_SHIFT 2


#define DBG_TCACHE_REQ_TYPE_RAW_READ 1
#define DBG_TCACHE_REQ_TYPE_RAW_WRITE 2
#define DBG_TCACHE_REQ_TYPE_FLUSH 3

/*
                                ~* Random Stuff *~
*/
#define SCRATCHPAD_MAX (268435456)
#define DRAM_TAG_STORE (0x8F000000)
#define DRAM_TAG_MASK (0xFFFFFF)

#endif
