lui t0 0xdeadb
addi t1 t1 0xee
slli t1 t1 4
addi t1 t1 0xf
add t0 t0 t1
sw t0 0 x0
lw t2 0 x0
addi t3 x0 4
sw t2 0 t3
a: j a