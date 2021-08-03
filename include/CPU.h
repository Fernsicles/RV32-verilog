#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <verilated.h>

#include "VCPU.h"

namespace RVGUI {
	class CPU {
		public:
			using Word = uint32_t;

			struct Options {
				std::string filename;
				size_t memorySize;
				bool separateInstructions = false, useTimeOffset = false;
				int32_t timeOffset = 0;
				uint32_t width = 480, height = 360;
				Word mmioOffset = 0x80'00'00'00;

				Options() = delete;
				Options(const std::string &filename_, size_t memory_size);
				Options & setSeparateInstructions(bool);
				Options & setTimeOffset(int32_t);
				Options & setWidth(uint32_t);
				Options & setHeight(uint32_t);
				Options & setDimensions(uint32_t width_, uint32_t height_);
				Options & setMMIOOffset(Word);
			};

			CPU() = delete;
			CPU(const Options &);

			/** Returns true if there are still more instructions to execute or false if the CPU has halted. */
			bool tick();

			void resetMemory();
			void loadProgram();
			void loadData(void *data, size_t size, size_t offset);

		private:
			Options options;
			std::unique_ptr<uint8_t[]> memory, framebuffer;
			std::unique_ptr<Word[]> instructions;
			std::unique_ptr<VCPU> vcpu;
			size_t count = 0;
			int64_t start = 0, end = 0;

			void init();
			void initFramebuffer(int channels = 3);
			void initVCPU();
	};
}
