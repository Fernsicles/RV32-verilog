VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	g++ -I obj_dir -I/usr/share/verilator/include ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

CPU.h: CPU.sv
ifeq ($(TEST), 1)
	verilator -cc CPU.sv
else
	verilator -Wall -cc CPU.sv
endif
	cd obj_dir && make -f VCPU.mk