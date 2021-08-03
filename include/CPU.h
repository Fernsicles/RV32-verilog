#pragma once

#include <cstdint>
#include <memory>
#include <verilated.h>

#include "VCPU.h"

namespace RVGUI {
	class CPU {
		public:
			CPU() = delete;
			CPU(size_t memory_size);

			/** Returns true if there are still more instructions to execute or false if the CPU has halted. */
			bool tick();

			void resetMemory();
			void loadData(void *data, size_t size, size_t offset);

		private:
			size_t memorySize;
			std::unique_ptr<uint8_t[]> memory;			
	};
}
