auipc t0 0
auipc t1 0
auipc t2 0
auipc t3 0
addi s0 x0 0x40
sw t0 0 s0
addi s0 s0 4
sw t1 0 s0
addi s0 s0 4
sw t2 0 s0
addi s0 s0 4
sw t3 0 s0
a: j a