#include <regex>

#include "lib/Disassembler.h"

namespace RVGUI {
	std::string disassemble(uint64_t pc, uint32_t instruction) {
		static char buffer[512];
		static std::regex start_regex("^[0-9a-f]+ +"), end_regex(" +# 0x[0-9a-f]+$");
		disasm_inst(buffer, sizeof(buffer), rv_isa::rv32, pc, instruction);
		std::string without_start, without_end, buffer_str = buffer;
		std::regex_replace(std::back_inserter(without_start), buffer_str.begin(), buffer_str.end(), start_regex, "");
		std::regex_replace(std::back_inserter(without_end), without_start.begin(), without_start.end(), end_regex, "");
		return without_end;
	}
}
