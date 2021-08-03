#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <verilated.h>

#include "VCPU.h"

namespace RVGUI {
	class CPU {
		public:
			CPU() = delete;
			CPU(const std::string &filename_, size_t memory_size, bool separate_instructions);
			CPU(const std::string &filename_, size_t memory_size, bool separate_instructions, int time_offset);

			/** Returns true if there are still more instructions to execute or false if the CPU has halted. */
			bool tick();

			void resetMemory();
			void loadProgram();
			void loadData(void *data, size_t size, size_t offset);

		private:
			std::string filename;
			size_t memorySize;
			int timeOffset;
			bool separateInstructions, useTimeOffset = false;
			std::unique_ptr<uint8_t[]> memory;
			std::unique_ptr<uint32_t[]> instructions;
			std::unique_ptr<VCPU> vcpu;

			void init();
			void initVCPU();
	};
}
