#include <cstring>
#include <stdexcept>

#include "CPU.h"

namespace RVGUI {
	CPU::CPU(const std::string &filename_, size_t memory_size, bool separate_instructions):
	filename(filename_), memorySize(memory_size), separateInstructions(separate_instructions) {
		init();
	}

	CPU::CPU(const std::string &filename_, size_t memory_size, bool separate_instructions, int time_offset):
	filename(filename_), memorySize(memory_size), separateInstructions(separate_instructions), timeOffset(time_offset),
	useTimeOffset(true) {
		init();
	}

	bool CPU::tick() {
		if (!vcpu)
			throw std::runtime_error("CPU isn't initialized");

		return vcpu->i_inst != 0x6f; // Jump to self
	}

	void CPU::resetMemory() {
		memory.reset(new uint8_t[memorySize]);
	}

	void CPU::loadProgram() {

	}

	void CPU::loadData(void *data, size_t size, size_t offset) {
		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");
		std::memcpy(memory.get() + offset, data, size);
	}

	void CPU::init() {
		resetMemory();
		loadProgram();
		initVCPU();
	}

	void CPU::initVCPU() {
		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");

		if (separateInstructions && !instructions)
			throw std::runtime_error("CPU instructions array isn't initialized");

		vcpu = std::make_unique<VCPU>();
		vcpu->i_clk = 0;
		vcpu->i_inst = 0x6f;
		vcpu->i_daddr = 0x2;
		vcpu->i_dload = 0x1;
		vcpu->i_ddata = memorySize - 1;
		vcpu->eval();
		vcpu->i_clk = 1;
		vcpu->eval();
		vcpu->i_clk = 0;
		vcpu->i_daddr = 0;
		vcpu->i_dload = 0;
		vcpu->i_ddata = 0;
		if (separateInstructions)
			vcpu->i_inst = ((uint32_t *) memory.get())[0];
		else
			vcpu->i_inst = instructions[0];
		vcpu->i_mem = memory[0];
		vcpu->eval();
	}
}
