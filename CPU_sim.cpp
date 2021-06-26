#include "obj_dir/VCPU.h"
#include "verilated.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char **argv) {
	Verilated::commandArgs(argc, argv);
	VCPU cpu;

	unsigned int inst[] = {
		0x0aa00493,
		0x01000913,
		0x00990023,
		0x012489b3,
		0x00499993,
		0x013910a3
	};

	uint8_t mem[64];
	fill(mem, mem + 64, 0);

	cpu.i_clk = 0;
	cpu.i_inst = inst[0];
	cpu.i_mem = mem[0];
	cpu.eval();

	for(int x = 0; x < 6; x++) {
		cpu.i_clk = 0;
		cpu.i_inst = inst[x];
		cpu.i_mem = cpu.o_addr;
		cpu.eval();
		cpu.i_clk = 1;
		cpu.eval();
		if(cpu.o_write) {
			switch(cpu.o_memsize) {
				case 1:
					mem[cpu.o_addr] = (uint8_t) cpu.o_mem;
					break;
				case 2:
					mem[cpu.o_addr] = (uint8_t) cpu.o_mem;
					mem[cpu.o_addr + 1] = (uint8_t) (cpu.o_mem >> 8);
					break;
				case 3:
					mem[cpu.o_addr] = (uint8_t) cpu.o_mem;
					mem[cpu.o_addr + 1] = (uint8_t) (cpu.o_mem >> 8);
					mem[cpu.o_addr + 2] = (uint8_t) (cpu.o_mem >> 8);
					break;
				default:
					break;
			}
		}
	}

	for(int x = 0; x < 64; x++) {
		cout << "0x";
		cout << setw(2) << setfill('0') << setbase(16) << (int) mem[x] << '\t';
		if((x + 1) % 16 == 0) {
			cout << endl;
		}
	}
	return 0;
}