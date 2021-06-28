#include "obj_dir/VCPU.h"
#include "verilated.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
#include <filesystem>

using namespace std;

int main(int argc, char **argv) {

	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int hflag = 0, pflag = 0, mflag = 0, vflag = 0;
	char *pstring;
	int memsize;
	static char usage[] = "usage: h [-h] [-v] [-i program] [-m memory_size]";
	while((c = getopt(argc, argv, "hp:m:v")) != -1) {
		switch(c) {
			case 'h':
				hflag = 1;
				break;
			case 'p':
				pflag = 1;
				pstring = optarg;
				break;
			case 'm':
				mflag = 1;
				memsize = stoi(optarg, nullptr);
				break;
			case 'v':
				vflag = 1;
				break;
			default:
				cout << usage << endl;
				abort();
		}
	}

	uint *inst;
	uint8_t *mem = (uint8_t *) calloc(memsize, sizeof(uint8_t));
	int filesize = filesystem::file_size(pstring);
	ifstream file;
	file.open(pstring, ios::in | ios::binary);
	if(vflag) {
		int i = 0;
		while(!file.eof() && i < filesize) {
			uint buf;
			file.read((char *) &buf, 1);
			mem[i] = buf;
			i++;
		}
	} else {
		int i = 0;
		inst = (uint *) calloc(filesize / 4, sizeof(uint));
		while(!file.eof() && i < filesize / 4) {
			int buf;
			file.read((char *) &buf, 4);
			inst[i] = buf;
			i++;
		}
	}

	if(vflag) {
		for(int x = 0; x < filesize / 4; x++) {
			cout << "0x";
			cout << setw(8) << setfill('0') << setbase(16) << ((uint *) mem)[x] << endl;
		}
	} else {
		for(int x = 0; x < filesize / 4; x++) {
			cout << "0x";
			cout << setw(8) << setfill('0') << setbase(16) << inst[x] << endl;
		}
	}
	

	Verilated::commandArgs(0, argv);
	VCPU cpu;

	cpu.i_clk = 0;
	if(vflag) {
		cpu.i_inst = ((uint *) mem)[0];
	} else {
		cpu.i_inst = inst[0];
	}
	cpu.i_mem = mem[0];
	cpu.eval();

	while(cpu.i_inst != 0x0000006f) {
		cpu.i_clk = 0;
		if(vflag) {
			cpu.i_inst = ((uint *) mem)[cpu.o_pc >> 2];
		} else {
			cpu.i_inst = inst[cpu.o_pc >> 2];
		}
		cpu.i_mem = ((uint *) mem)[cpu.o_addr % memsize];
		cpu.eval();
		cpu.i_clk = 1;
		cpu.eval();
		if(cpu.o_write) {
			switch(cpu.o_memsize) {
				case 1:
					mem[cpu.o_addr] = (uint8_t) cpu.o_mem;
					break;
				case 2:
					((uint16_t *) mem)[cpu.o_addr] = (uint16_t) cpu.o_mem;
					break;
				case 3:
					((uint32_t *) mem)[cpu.o_addr] = (uint32_t) cpu.o_mem;
					break;
				default:
					break;
			}
		}
	}

	for(int x = 0; x < memsize; x++) {
		cout << "0x";
		cout << setw(2) << setfill('0') << setbase(16) << (int) mem[x] << '\t';
		if((x + 1) % 4 == 0) {
			cout << endl;
		}
	}
	return 0;
}