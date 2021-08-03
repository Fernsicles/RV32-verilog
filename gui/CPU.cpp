#include <chrono>
#include <cstring>
#include <stdexcept>

#include "CPU.h"

namespace RVGUI {
	CPU::Options::Options(const std::string &filename_, size_t memory_size):
		filename(filename_), memorySize(memory_size) {}

	CPU::Options & CPU::Options::setSeparateInstructions(bool value) {
		separateInstructions = value;
		return *this;
	}

	CPU::Options & CPU::Options::setTimeOffset(int32_t value) {
		timeOffset = value;
		useTimeOffset = true;
		return *this;
	}

	CPU::Options & CPU::Options::setWidth(uint32_t value) {
		width = value;
		return *this;
	}

	CPU::Options & CPU::Options::setHeight(uint32_t value) {
		height = value;
		return *this;
	}

	CPU::Options & CPU::Options::setDimensions(uint32_t width_, uint32_t height_) {
		width = width_;
		height = height_;
		return *this;
	}

	CPU::Options & CPU::Options::setMMIOOffset(Word value) {
		mmioOffset = value;
		return *this;
	}

	CPU::CPU(const Options &options_): options(options_) {
		init();
	}

	bool CPU::tick() {
		if (!vcpu)
			throw std::runtime_error("CPU isn't initialized");

		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");

		if (options.separateInstructions && !instructions)
			throw std::runtime_error("CPU instructions array isn't initialized");

		if (options.useTimeOffset)
			*reinterpret_cast<int64_t *>(memory.get() + options.timeOffset) =
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		vcpu->i_clk = 0;

		if (options.separateInstructions)
			vcpu->i_inst = reinterpret_cast<Word *>(memory.get())[vcpu->o_pc / sizeof(Word)];
		else
			vcpu->i_inst = instructions[vcpu->o_pc / sizeof(Word)];

		vcpu->eval();

		if (vcpu->o_load) {
			if (options.mmioOffset <= vcpu->o_addr) {
				std::memcpy(&vcpu->i_mem, framebuffer.get() + vcpu->o_addr - options.mmioOffset, sizeof(Word));
			} else {
				std::memcpy(&vcpu->i_mem, memory.get() + vcpu->o_addr % options.memorySize, sizeof(Word));
			}
		}

		return vcpu->i_inst != 0x6f; // Jump to self
	}

	void CPU::resetMemory() {
		memory.reset(new uint8_t[options.memorySize]);
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
		initFramebuffer(3);
		loadProgram();
		initVCPU();
	}

	void CPU::initFramebuffer(int channels) {
		if (options.width == 0 && options.height == 0)
			framebuffer.reset();
		else if (options.width != 0 && options.height != 0)
			framebuffer.reset(new uint8_t[options.width * options.height * channels]);
		else
			throw std::invalid_argument("Exactly one of width and height is zero");
	}

	void CPU::initVCPU() {
		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");

		if (options.separateInstructions && !instructions)
			throw std::runtime_error("CPU instructions array isn't initialized");

		vcpu = std::make_unique<VCPU>();
		vcpu->i_clk = 0;
		vcpu->i_inst = 0x6f;
		vcpu->i_daddr = 0x2;
		vcpu->i_dload = 0x1;
		vcpu->i_ddata = options.memorySize - 1;
		vcpu->eval();
		vcpu->i_clk = 1;
		vcpu->eval();
		vcpu->i_clk = 0;
		vcpu->i_daddr = 0;
		vcpu->i_dload = 0;
		vcpu->i_ddata = 0;
		if (options.separateInstructions)
			vcpu->i_inst = ((Word *) memory.get())[0];
		else
			vcpu->i_inst = instructions[0];
		vcpu->i_mem = memory[0];
		vcpu->eval();
	}
}
