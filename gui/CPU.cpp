#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "CPU.h"
#include "MMIO.h"
#include "Util.h"

namespace RVGUI {
	CPU::Options::Options(const std::string &program_filename, size_t memory_size):
		programFilename(program_filename), memorySize(memory_size) {}

	CPU::Options & CPU::Options::setDataFilename(const std::string &value) {
		dataFilename = value;
		return *this;
	}

	CPU::Options & CPU::Options::setDataFilename(const std::string &value, Word offset) {
		dataFilename = value;
		dataOffset = offset;
		return *this;
	}

	CPU::Options & CPU::Options::setDataOffset(Word value) {
		dataOffset = value;
		return *this;
	}

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

	CPU::Options & CPU::Options::setVideoMode(VideoMode value) {
		videoMode = value;
		return *this;
	}

	CPU::CPU(const Options &options_): options(options_) {
		init();
	}

	CPU::TickResult CPU::tick() {
		if (!vcpu)
			throw std::runtime_error("CPU isn't initialized");

		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");

		if (options.separateInstructions && !instructions)
			throw std::runtime_error("CPU instructions array isn't initialized");

		auto lock = lockCPU();

		if (start == 0)
			start = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		if (options.useTimeOffset)
			*reinterpret_cast<int64_t *>(memory.get() + options.timeOffset) =
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		vcpu->i_clk = 0;

		if (options.separateInstructions)
			vcpu->i_inst = instructions[vcpu->o_pc / sizeof(Word)];
		else
			vcpu->i_inst = reinterpret_cast<Word *>(memory.get())[vcpu->o_pc / sizeof(Word)];

		vcpu->eval();

		if (vcpu->o_load) {
			if (options.mmioOffset + FRAMEBUFFER_OFFSET <= vcpu->o_addr) {
				const ptrdiff_t ptrsum = vcpu->o_addr - options.mmioOffset - FRAMEBUFFER_OFFSET;
				const uintptr_t fbstart = (uintptr_t) framebuffer.get(), fbend = fbstart + framebufferSize;
				if (!(0 <= ptrsum && ptrsum + 3 < framebufferSize)) {
					std::cerr << "Framebuffer: [" << toHex(fbstart) << ", " << toHex(fbend) << ")\n";
					throw std::out_of_range("Framebuffer read of size 4 out of range (" + toHex(fbstart + ptrsum)
						+ ")");
				}
				vcpu->i_mem = *reinterpret_cast<Word *>(framebuffer.get() + ptrsum);
			} else if (vcpu->o_addr == options.mmioOffset + FRAMEBUFFER_READY) {
				vcpu->i_mem = framebufferReady;
			} else if (vcpu->o_addr == options.mmioOffset + KEYVAL) {
				vcpu->i_mem = lastKeyValue;
				lastKeyValue = '\0';
			} else if (options.mmioOffset <= vcpu->o_addr && vcpu->o_addr < options.mmioOffset + MMIO_END) {
				throw std::out_of_range("Invalid read from MMIO location " + toHex(vcpu->o_addr - options.mmioOffset));
			} else {
				const ptrdiff_t ptr = vcpu->o_addr % options.memorySize;
				const uintptr_t memstart = (uintptr_t) memory.get(), memend = memstart + options.memorySize;
				if (ptr == options.memorySize - 1) {
					vcpu->i_mem = memory[ptr];
				} else if (ptr == options.memorySize - 2) {
					vcpu->i_mem = *reinterpret_cast<uint16_t *>(memory.get() + ptr);
				} else if (!(0 <= ptr && ptr + 3 < options.memorySize)) {
					std::cerr << "Memory: [" << toHex(memstart) << ", " << toHex(memend) << ")\n";
					throw std::out_of_range("Memory read of size 4 out of range (" + toHex(memstart + ptr) + ")");
				} else {
					vcpu->i_mem = *reinterpret_cast<Word *>(memory.get() + vcpu->o_addr % options.memorySize);
				}
			}
		}

		vcpu->eval();
		vcpu->i_clk = 1;
		vcpu->eval();

		uint8_t *pointer;
		Word address;

		bool normal_write = false;

		if (options.mmioOffset + FRAMEBUFFER_OFFSET <= vcpu->o_addr) {
			pointer = framebuffer.get();
			address = vcpu->o_addr - options.mmioOffset - FRAMEBUFFER_OFFSET;
			normal_write = true;
		} else if (vcpu->o_addr < options.mmioOffset) {
			pointer = memory.get();
			address = vcpu->o_addr;
			normal_write = true;
		}

		if (vcpu->o_write) {
			if (normal_write) {
				uint8_t *ptrsum = pointer + address;
				const uint8_t *memstart = memory.get(), *memend = memstart + options.memorySize;
				const uint8_t *fbstart = framebuffer.get(), *fbend = fbstart + framebufferSize;
				switch (vcpu->o_memsize) {
					case 1:
						if (!(memstart <= ptrsum && ptrsum < memend) && !(fbstart <= ptrsum && ptrsum < fbend))
							throw std::out_of_range("Write of size 1 out of range (" + toHex(ptrsum) + ")");
						*ptrsum = vcpu->o_mem;
						break;
					case 2:
						if (!(memstart <= ptrsum && ptrsum + 1 < memend) && !(fbstart <= ptrsum && ptrsum + 1 < fbend))
							throw std::out_of_range("Write of size 2 out of range (" + toHex(ptrsum) + ")");
						*reinterpret_cast<uint16_t *>(ptrsum) = vcpu->o_mem;
						break;
					case 3:
						if (!(memstart <= ptrsum && ptrsum + 3 < memend) && !(fbstart <= ptrsum && ptrsum + 3 < fbend)) {
							std::cerr << "Memory: [" << toHex(memstart) << ", " << toHex(memend) << ")\n";
							throw std::out_of_range("Write of size 4 out of range (" + toHex(ptrsum) + ")");
						}
						*reinterpret_cast<uint32_t *>(ptrsum) = vcpu->o_mem;
						break;
					default:
						break;
				}
			} else if (options.mmioOffset <= vcpu->o_addr) {
				if (vcpu->o_addr == options.mmioOffset + FRAMEBUFFER_READY) {
					if (vcpu->o_memsize != 1)
						throw std::runtime_error("Invalid write size (" + std::to_string(vcpu->o_memsize)
							+ ") to FRAMEBUFFER_READY");
					framebufferReady = vcpu->o_mem;
				} else if (vcpu->o_addr == options.mmioOffset + PUTCHAR) {
					if (onPrint && options.videoMode == VideoMode::Text && vcpu->o_mem != 0)
						onPrint(static_cast<char>(vcpu->o_mem));
				} else if (vcpu->o_addr == options.mmioOffset + KEYPAUSE) {
					if (vcpu->o_mem) {
						++count;
						return TickResult::KeyPause;
					}
				} else
					throw std::out_of_range("Invalid MMIO write to " + toHex(vcpu->o_addr));
			} else
				throw std::out_of_range("Invalid write to " + toHex(vcpu->o_addr));
		}

		++count;

		if (vcpu->i_inst == 0x6f) { // Jump to self
			 end = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			return TickResult::Finished;
		}

		return TickResult::Continue;
	}

	void CPU::resetMemory() {
		memory.reset(new uint8_t[options.memorySize]());
	}

	void CPU::loadProgram() {
		if (options.programFilename.empty())
			throw std::runtime_error("Program filename is empty");

		std::ifstream file;
		file.open(options.programFilename, std::ios::in | std::ios::binary);

		const auto filesize = std::filesystem::file_size(options.programFilename);

		if (!file.is_open())
			throw std::runtime_error("Failed to open program for reading");

		char *buffer = new char[filesize];
		file.read(buffer, filesize);

		if (4 < filesize && buffer[0] == 0x7f && buffer[1] == 'E' && buffer[2] == 'L' && buffer[3] == 'F') {
			if (!elfReader.load(options.programFilename))
				throw std::runtime_error("Couldn't load ELF binary " + options.programFilename);
			std::cout << "ELF detected\n";
			if (elfReader.get_class() != ELFCLASS32)
				throw std::runtime_error("Unsupported ELF binary (not elf32)");
			std::memcpy(memory.get(), buffer, filesize);
			const auto &text = *elfReader.sections[".text"];
			textOffset = text.get_offset();
			instructionCount = text.get_size() / sizeof(Word);
		} else {
			instructionCount = filesize / sizeof(Word);
			if (options.separateInstructions) {
				instructions.reset(new Word[instructionCount]);
				std::memcpy(instructions.get(), buffer, filesize);
			} else {
				instructions.reset();
				std::memcpy(memory.get(), buffer, filesize);
			}
		}
	}

	void CPU::loadData(void *data, size_t size, size_t offset) {
		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");
		std::memcpy(memory.get() + offset, data, size);
	}

	void CPU::loadData() {
		if (options.dataFilename.empty())
			return;
		const auto datasize = std::filesystem::file_size(options.dataFilename);
		std::ifstream data;
		data.open(options.dataFilename, std::ios::in | std::ios::binary);

		if (options.memorySize < options.dataOffset + datasize)
			throw std::runtime_error("Can't load data: not enough space in memory");

		std::cout << "Loading data...\n";
		data.read(reinterpret_cast<char *>(memory.get()) + options.dataOffset, datasize);
		std::cout << "Data loaded.\n";
	}

	Word CPU::getPC() const {
		return vcpu? vcpu->o_pc : 0;
	}

	void CPU::setPC(Word new_pc) {
		if (vcpu) {
			// TODO: verify
			const auto old_instruction = vcpu->i_inst;
			vcpu->i_inst = 0x6f;
			vcpu->i_clk = 0;
			vcpu->i_pcload = 1;
			vcpu->i_pc = new_pc;
			vcpu->eval();
			vcpu->i_clk = 1;
			vcpu->eval();
			vcpu->i_pcload = 0;
			vcpu->i_inst = old_instruction;
		}
	}

	Word CPU::getRegister(uint8_t reg) {
		if (32 <= reg)
			throw std::out_of_range("Invalid register index: " + std::to_string(reg));
		return vcpu? vcpu->o_reg[reg] : 0;
	}

	void CPU::setRegister(uint8_t reg, Word value) {
		if (32 <= reg)
			throw std::out_of_range("Invalid register index: " + std::to_string(reg));
		if (!vcpu)
			return;
		auto lock = lockCPU();
		// TODO: verify
		const auto old_instruction = vcpu->i_inst;
		vcpu->i_inst = 0x6f;
		vcpu->i_clk = 0;
		vcpu->i_daddr = reg;
		vcpu->i_ddata = value;
		vcpu->i_dload = 1;
		vcpu->eval();
		vcpu->i_clk = 1;
		vcpu->eval();
		vcpu->i_dload = 0;
		vcpu->i_inst = old_instruction;
	}

	const Word * CPU::getInstructions() const {
		if (options.separateInstructions)
			return instructions.get();
		return reinterpret_cast<Word *>(memory.get() + textOffset);
	}

	void CPU::init() {
		resetMemory();
		initFramebuffer(3);
		loadProgram();
		loadData();
		initVCPU();
		setPC(textOffset);
	}

	void CPU::initFramebuffer(int channels) {
		if (options.width == 0 && options.height == 0) {
			framebuffer.reset();
			framebufferSize = 0;
		} else if (options.width != 0 && options.height != 0) {
			framebuffer.reset(new uint8_t[framebufferSize = options.width * options.height * channels]());
		} else
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
			vcpu->i_inst = instructions[textOffset / sizeof(Word)];
		else
			vcpu->i_inst = ((Word *) memory.get())[textOffset / sizeof(Word)];
		vcpu->i_mem = memory[0];
		vcpu->eval();
	}
}
