#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>

#include <CImg.h>
#include <verilated.h>

#include "obj_dir/VCPU.h"

using namespace cimg_library;

std::mutex update_mutex;
bool update_done = false;

void update_window(CImgDisplay *display, CImg<uint8_t> *image, int *fps) {
	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / *fps)); // Only update the window at the specified framerate
		display->display(*image);
		auto lock = std::unique_lock(update_mutex);
		if (update_done)
			break;
	}
}

int main(int argc, char **argv) {
	extern char *optarg;
	int c;
	int hflag       = 0; // -h: Show help
	int Dflag       = 0; // -D: Display memory after execution
	int pflag       = 0; // -p: Program path
	int mflag       = 0; // -m: Memory size
	int vflag       = 0; // -v: Read instructions into memory, instead of into a separate array
	int dflag       = 0; // -d: Path of data to load into memory
	int Tflag       = 0; // -T: Time offset of clock
	int data_offset = 0; // -t: Offset of data loaded into memory (see -d)
	int time_offset = 0; // -T: Time offset of clock
	uint32_t width = 480, height = 360, offset = 0x80'00'00'00;
	int framerate = 30;
	char *pstring, *dstring;
	uint64_t memsize = 0;
	static char usage[] = "Usage: h [-h] [-v] [-D] [-p program] [-m memory_size] [-x width] [-y height] [-f framerate] [-o mmio_offset] [-d data] [-t data_offset] [-T time_offset]";
	while ((c = getopt(argc, argv, "hDp:m:vx:y:f:d:t:T:")) != -1) {
		switch(c) {
			case 'h':
				hflag = 1;
				break;
			case 'D':
				Dflag = 1;
				break;
			case 'p':
				pflag = 1;
				pstring = optarg;
				break;
			case 'm':
				mflag = 1;
				memsize = std::stoul(optarg, nullptr);
				break;
			case 'v':
				vflag = 1;
				break;
			case 'x':
				width = std::stoi(optarg, nullptr);
				break;
			case 'y':
				height = std::stoi(optarg, nullptr);
				break;
			case 'f':
				framerate = std::stoi(optarg, nullptr);
				break;
			case 'o':
				offset = std::stoi(optarg, nullptr);
				break;
			case 'd':
				dflag = 1;
				dstring = optarg;
				break;
			case 't':
				data_offset = std::stoi(optarg, nullptr);
				break;
			case 'T':
				Tflag = 1;
				time_offset = std::stoi(optarg, nullptr);
				break;
			default:
				std::cout << usage << std::endl;
				return 1;
		}
	}

	if (hflag) {
		std::cout << usage << std::endl;
		return 1;
	}

	if (!pflag || !mflag) {
		std::cerr << "ERROR: Both a program and memory size are required." << std::endl;
		std::cout << usage << std::endl;
		return 1;
	}

	// This is mostly because I couldn't be bothered with a less hacky check when writing to the framebuffer
	if (offset < memsize) {
		std::cerr << "ERROR: Memory overlaps with MMIO." << std::endl;
		return 1;
	}

	CImg<uint8_t> fb(width, height, 1, 3, 0);
	CImgDisplay window(fb, "Frame Buffer", 0);
	std::thread update(update_window, &window, &fb, &framerate);

	// Load program into memory
	uint32_t *inst = nullptr;
	uint8_t *mem = (uint8_t *) calloc(memsize, sizeof(uint8_t));
	uint32_t filesize = std::filesystem::file_size(pstring);
	std::ifstream file;
	file.open(pstring, std::ios::in | std::ios::binary);
	if (vflag) { // Von Neumann architecture, load program into main memory
		for (uint32_t i = 0; !file.eof() && i < filesize; ++i)
			file.read(reinterpret_cast<char *>(&mem[i]), 1);
	} else {     // Harvard architecture, load program into program memory
		inst = (uint32_t *) calloc(filesize / 4, sizeof(uint32_t));
		for (uint32_t i = 0; !file.eof() && i < filesize / 4; ++i)
			file.read(reinterpret_cast<char *>(&inst[i]), 4);
	}

	// Print out the loaded program, this is so the user can visually check that loading was successful
	if (vflag) {
		for (uint32_t x = 0; x < filesize / 4; ++x) {
			std::cout << "0x";
			std::cout << std::setw(8) << std::setfill('0') << std::setbase(16) << ((uint32_t *) mem)[x] << std::endl;
		}
	} else {
		for (uint32_t x = 0; x < filesize / 4; ++x) {
			std::cout << "0x";
			std::cout << std::setw(8) << std::setfill('0') << std::setbase(16) << inst[x] << std::endl;
		}
	}
	
	// Load data into memory
	if (dflag) {
		uint32_t datasize = std::filesystem::file_size(dstring);
		std::ifstream data;
		data.open(dstring, std::ios::in | std::ios::binary);
		for (uint32_t i = 0; !data.eof() && i < datasize; ++i) {
			data.read(reinterpret_cast<char *>(mem) + data_offset + i, 1);
		}
		std::cout << "Finished loading data." << std::endl;
	}

	Verilated::commandArgs(0, argv);
	VCPU cpu;

	// Timer to measure runtime
	uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	// Initialize stack pointer to top of memory
	cpu.i_clk = 0;
	cpu.i_inst = 0x6f;
	cpu.i_daddr = 0x2;         // Select SP register
	cpu.i_dload = 0x1;
	cpu.i_ddata = memsize - 1; // Load last valid, non-MMIO memory address into SP
	cpu.eval();
	cpu.i_clk = 1;
	cpu.eval();
	cpu.i_clk = 0;
	cpu.i_daddr = 0;
	cpu.i_dload = 0;
	cpu.i_ddata = 0;

	// Instruction fetch
	if(vflag) {
		cpu.i_inst = ((uint32_t *) mem)[0];
	} else {
		cpu.i_inst = inst[0];
	}
	cpu.i_mem = mem[0];
	cpu.eval();
	uint8_t *pointer;
	uint32_t addr;
	uint64_t count = 0;

	while (cpu.i_inst != 0x6f) { // 0x6f is jump to current instruction, essentially an inescapable loop. Here we use it to terminate programs.
		// If clock is enabled, write the current time to the specified offset
		if (Tflag) {
			unsigned int t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			((unsigned int *) mem)[time_offset] = t;
		}

		cpu.i_clk = 0;

		// Instruction fetch
		if (vflag) {
			cpu.i_inst = ((uint32_t *) mem)[cpu.o_pc >> 2];
		} else {
			cpu.i_inst = inst[cpu.o_pc >> 2];
		}
		cpu.eval();

		// Load data from memory
		if(cpu.o_load) {
			if(cpu.o_addr >= offset) {
				memcpy(&cpu.i_mem, fb.data() + cpu.o_addr - offset, 4);
			} else {
				memcpy(&cpu.i_mem, mem + cpu.o_addr % memsize, 4);
			}
		}
		cpu.eval();
		cpu.i_clk = 1;
		cpu.eval();
		
		// Select whether memory writes go to the framebuffer or main memory
		if (cpu.o_addr >= offset) {
			pointer = fb.data();
			addr = cpu.o_addr - offset;
		} else {
			pointer = mem;
			addr = cpu.o_addr;
		}

		// Write to memory
		if (cpu.o_write) {
			switch (cpu.o_memsize) {
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

		++count;
	}

	uint64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	// Print out memory, this can potentially output a lot of stuff
	if (Dflag)
		for (uint64_t i = 0; i < memsize; ++i) {
			std::cout << "0x";
			std::cout << std::setw(2) << std::setfill('0') << std::setbase(16) << (int) mem[i] << '\t';
			if ((i + 1) % 4 == 0)
				std::cout << std::endl;
		}

	std::cout << std::setbase(10) << "Time elapsed: " << end - start << " ms" << std::endl << "Cycles: " << count << std::endl;

	free(mem);
	if (!vflag)
		free(inst);

	{
		auto lock = std::unique_lock(update_mutex);
		update_done = true;
	}

	update.join();
	return 0;
}