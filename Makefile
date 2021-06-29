VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	g++ -I obj_dir -I/usr/share/verilator/include ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

obj_dir/VCPU.h: CPU.sv control_unit.sv register.sv ALU.sv
ifeq ($(TEST), 1)
	verilator -cc CPU.sv
else
	verilator -Wall -cc CPU.sv
endif
	cd obj_dir && make -f VCPU.mk

verilated.o:
	g++ -c /usr/share/verilator/include/verilated.cpp

CPU_sim: obj_dir/VCPU.h CPU_sim.cpp verilated.o
	g++ -Wall -g -pthread -Dcimg_display=1 -I obj_dir -I/usr/share/verilator/include CPU_sim.cpp verilated.o obj_dir/VCPU__ALL.o /usr/lib/libX11.so -o CPU_sim