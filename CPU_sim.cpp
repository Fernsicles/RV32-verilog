#include "obj_dir/VCPU.h"
#include "verilated.h"
#include <CImg.h>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace cimg_library;

int main(int argc, char **argv) {

	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int hflag = 0, pflag = 0, mflag = 0, vflag = 0, dflag = 0, Tflag = 0, data_offset = 0, time_offset;
	uint width = 480, height = 360;
	uint offset = 0x80000000;
	uint frames = 10000;
	char *pstring;
	char * dstring;
	ulong memsize;
	static char usage[] = "Usage: h [-h] [-v] [-p program] [-m memory_size] [-x width] [-y height] [-o mmio_offset] [-f cycles/frame] [-d data] [-t data_offset] [-T time_offset]";
	while((c = getopt(argc, argv, "hp:m:vx:y:f:d:t:T:")) != -1) {
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
				memsize = stoul(optarg, nullptr);
				break;
			case 'v':
				vflag = 1;
				break;
			case 'x':
				width = stoi(optarg, nullptr);
				break;
			case 'y':
				height = stoi(optarg, nullptr);
				break;
			case 'o':
				offset = stoi(optarg, nullptr);
				break;
			case 'f':
				frames = stoi(optarg, nullptr);
				break;
			case 'd':
				dflag = 1;
				dstring = optarg;
				break;
			case 't':
				data_offset = stoi(optarg, nullptr);
				break;
			case 'T':
				Tflag = 1;
				time_offset = stoi(optarg, nullptr);
				break;
			default:
				cout << usage << endl;
				abort();
		}
	}

	if(hflag) {
		cout << usage << endl;
		abort();
	}
	if(!pflag || !mflag) {
		cerr << "ERROR: Both a program and memory size are required." << endl;
		cout << usage << endl;
		abort();
	}

	if(memsize > offset) {
		cerr << "ERROR: Memory overlaps with MMIO.";
		abort();
	}

	CImg<uint8_t> fb(width, height, 1, 3, 0);
	CImgDisplay window(fb, "Frame Buffer", 0);

	uint *inst;
	uint8_t *mem = (uint8_t *) calloc(memsize, sizeof(uint8_t));
	uint filesize = filesystem::file_size(pstring);
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
	
	if(dflag) {
		uint datasize = filesystem::file_size(dstring);
		ifstream data;
		data.open(dstring, ios::in | ios::binary);
		int i = 0;
		while(!data.eof() && i < datasize) {
			// char buf;
			data.read((char *) mem + data_offset + i, 1);
			// memcpy(mem + data_offset + i, buf, 1);
			// mem[data_offset + i] = buf;
			i++;
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
	uint8_t *pointer;
	uint addr;
	unsigned long count = 0;
	while(cpu.i_inst != 0x0000006f) {

		if(Tflag) {
			int t = time(nullptr);
			memcpy(mem + time_offset, &t, 4);
		}

		cpu.i_clk = 0;
		if(vflag) {
			cpu.i_inst = ((uint *) mem)[cpu.o_pc >> 2];
		} else {
			cpu.i_inst = inst[cpu.o_pc >> 2];
		}
		cpu.eval();
		if(cpu.o_addr >= offset && cpu.o_load) {
			memcpy(&cpu.i_mem, fb.data() + cpu.o_addr - offset, 4);
		} else {
			memcpy(&cpu.i_mem, mem + cpu.o_addr % memsize, 4);
		}
		cpu.eval();
		cpu.i_clk = 1;
		cpu.eval();
		
		if(cpu.o_addr >= offset) {
			pointer = fb.data();
			addr = cpu.o_addr - offset;
		} else {
			pointer = mem;
			addr = cpu.o_addr;
		}
		if(cpu.o_write) {
			switch(cpu.o_memsize) {
				case 1:
					memcpy(pointer + addr, &cpu.o_mem, 1);
					break;
				case 2:
					memcpy(pointer + addr, &cpu.o_mem, 2);
					break;
				case 3:
					memcpy(pointer + addr, &cpu.o_mem, 4);
					break;
				default:
					break;
			}
		}
		if(count % frames == 0) {
			window.display(fb);
		}
		count++;
	}

	for(int x = 0; x < memsize; x++) {
		cout << "0x";
		cout << setw(2) << setfill('0') << setbase(16) << (int) mem[x] << '\t';
		if((x + 1) % 4 == 0) {
			cout << endl;
		}
	}

	free(mem);
	if(!vflag) {
		free(inst);
	}
	return 0;
}