li t0 0
li s0 172800
add t1 x0 s0
add s0 t1 s0
add s0 t1 s0
li t1 0xFF
li s1 0x80000000
a:
sb t1 0 s1
addi t0 t0 1
addi s1 s1 1
blt t0 s0 a
b: jal x1 b