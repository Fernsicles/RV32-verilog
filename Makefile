VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	g++ -I obj_dir -I/usr/share/verilator/include ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

obj_dir/VCPU.h: CPU.sv control_unit.sv register.sv ALU.sv
ifeq ($(TEST), 1)
	verilator -O3 -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
else
	verilator -Wall -O3 -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
endif

verilated.o: /usr/share/verilator/include/verilated.cpp
	g++ -c -O5 /usr/share/verilator/include/verilated.cpp

CPU_sim: obj_dir/VCPU.h CPU_sim.cpp verilated.o
	g++ -Wall -g -O5 -pthread -Dcimg_display=1 -I obj_dir -I/usr/share/verilator/include CPU_sim.cpp verilated.o obj_dir/VCPU__ALL.o /usr/lib/libX11.so -o CPU_sim