#pragma once

#include <string>

#include "riscv-disas.h"

namespace RVGUI {
	std::string disassemble(uint64_t pc, uint32_t instruction);
}
