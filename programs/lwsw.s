.text
la t0 a
lw t1 0 t0
addi t2 x0 0x50
sw t1 0 t2
addi t2 t2 4
addi t0 t0 4
lw t1 0 t0
sw t1 0 t2
c: j c
a: .word 0x01234567
.word 0x89ABCDEF