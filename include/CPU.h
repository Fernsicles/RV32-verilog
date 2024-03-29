#pragma once

#include <gtkmm.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <verilated.h>

#include "Defs.h"
#include "VCPU.h"
#include "elfio/elfio.hpp"

namespace RVGUI {
	class CPU {
		public:
			enum class TickResult {Continue, Finished, KeyPause};

			struct Options {
				std::string programFilename, dataFilename;
				size_t memorySize;
				bool separateInstructions = false, useTimeOffset = false;
				int32_t timeOffset = 0;
				uint32_t width = 480, height = 360;
				Word mmioOffset = 0x80'00'00'00;
				Word dataOffset = 0;
				VideoMode videoMode = VideoMode::RGB;

				Options() = delete;
				Options(const std::string &program_filename, size_t memory_size);
				Options & setDataFilename(const std::string &);
				Options & setDataFilename(const std::string &, Word offset);
				Options & setDataOffset(Word);
				Options & setSeparateInstructions(bool);
				Options & setTimeOffset(int32_t);
				Options & setWidth(uint32_t);
				Options & setHeight(uint32_t);
				Options & setDimensions(uint32_t width_, uint32_t height_);
				Options & setMMIOOffset(Word);
				Options & setVideoMode(VideoMode);
			};

			bool useLock = true;
			uint8_t framebufferReady = 0;
			guint lastKeyValue = '\0';
			std::function<void(char)> onPrint;

			CPU() = delete;
			CPU(const Options &);

			TickResult tick();

			void resetMemory();
			void loadProgram();
			void loadData(void *data, size_t size, size_t offset);
			void loadData();
			Word getPC() const;
			void setPC(Word);
			Word getRegister(uint8_t reg);
			void setRegister(uint8_t reg, Word value);
			const Word * getInstructions() const;
			uint8_t * getMemory() const { return memory.get(); }
			size_t getInstructionCount() const { return instructionCount; }
			size_t getInstructionOffset() const { return textOffset; }
			size_t memorySize() const { return options.memorySize; }
			uint8_t operator[](size_t offset) const { return memory[offset]; }
			const Options & getOptions() const { return options; }
			uint8_t * getFramebuffer() const { return framebuffer.get(); }
			size_t getCount() const { return count; }

			inline std::unique_lock<std::mutex> lockCPU() {
				return useLock? std::unique_lock(mutex) : std::unique_lock<std::mutex>();
			}

		private:
			static constexpr size_t FRAMEBUFFER_OFFSET = 0x01'00'00'00; // 16 mibibytes
			Options options;
			std::unique_ptr<uint8_t[]> memory;
			std::shared_ptr<uint8_t[]> framebuffer;
			std::unique_ptr<Word[]> instructions;
			std::unique_ptr<VCPU> vcpu;
			std::mutex mutex;
			ELFIO::elfio elfReader;
			size_t count = 0, instructionCount = 0, framebufferSize = 0, textOffset = 0;
			int64_t start = 0, end = 0;

			void init();
			void initFramebuffer(int channels = 3);
			void initVCPU();
	};
}
