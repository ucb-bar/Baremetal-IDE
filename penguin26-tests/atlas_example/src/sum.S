# sum.S - Atlas program used by the atlas_example test (reference only; not built).
# Adds 1..10 on the Atlas scalar core, puts the result in DBG1 and signals "done" through DBG0.
# Encoded with generators/atlas-npu/baremetal/assembler.py; the words are in src/main.c.
# Branches and jumps have 2 delay slots (the 2 instructions after them always execute).

    ADDI  x1, x0, 0         # x1 = sum = 0
    ADDI  x2, x0, 10        # x2 = i = 10
loop:
    ADD   x1, x1, x2        # sum += i
    ADDI  x2, x2, -1        # i--
    BNE   x2, x0, loop      # repeat while i != 0
    NOP                     # delay slot 1
    NOP                     # delay slot 2
    CSRRW x0, 0xC11, x1     # DBG1 = sum (expected 55)
    ADDI  x3, x0, 1
    CSRRW x0, 0xC10, x3     # DBG0 = 1: tells the host the program finished
    ECALL                   # halt (halt_reason = ecall)
