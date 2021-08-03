OPTIMIZATION ?= -O3

.PHONY: all clean

all: CPU_sim

obj_dir/VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	g++ -std=c++17 -Iobj_dir $(shell pkg-config --cflags verilator) ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

obj_dir/VCPU.h: CPU.sv control_unit.sv registers.sv ALU.sv
ifeq ($(TEST), 1)
	verilator $(OPTIMIZATION) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
else
	verilator -Wall $(OPTIMIZATION) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
endif

verilated.o: /usr/share/verilator/include/verilated.cpp
	g++ -std=c++17 -c $(OPTIMIZATION) /usr/share/verilator/include/verilated.cpp

CPU_sim: obj_dir/VCPU.h CPU_sim.cpp verilated.o
	g++ -std=c++17 -Wall -g $(OPTIMIZATION) -pthread -Dcimg_display=1 -Iobj_dir $(shell pkg-config --cflags verilator) CPU_sim.cpp verilated.o obj_dir/VCPU__ALL.o $(shell pkg-config --libs x11) -o CPU_sim

clean:
	rm -rf obj_dir verilated.o CPU_sim
