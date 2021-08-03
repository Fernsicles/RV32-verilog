#include <cstring>
#include <stdexcept>

#include "CPU.h"

namespace RVGUI {
	CPU::CPU(size_t memory_size): memorySize(memory_size) {
		resetMemory();
	}

	bool CPU::tick() {

	}

	void CPU::resetMemory() {
		memory.reset(new uint8_t[memorySize]);
	}

	void CPU::loadData(void *data, size_t size, size_t offset) {
		if (!memory)
			throw std::runtime_error("CPU memory isn't initialized");
		std::memcpy(memory.get() + offset, data, size);
	}
}
