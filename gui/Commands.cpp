#include <unordered_map>

#include "CPU.h"
#include "Ustring.h"
#include "Util.h"
#include "ui/MainWindow.h"

namespace RVGUI {
	void MainWindow::addCommands() {
		console.addCommand("echo", [](Console &console, auto &pieces) {
			console.append(join(pieces.begin() + 1, pieces.end(), " "));
		});

		console.addCommand("rreg", [this](Console &console, auto &pieces) {
			if (!cpu) {
				console.append("No active CPU.");
				return;
			}

			if (pieces.size() != 2) {
				console.append("Usage: rreg <register>");
				return;
			}

			const auto &reg_str = pieces[1];

			static const std::unordered_map<Glib::ustring, uint8_t> registers {
				{"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8},
				{"fp", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15},
				{"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
				{"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31},
			};

			uint64_t reg = UINT64_MAX;
			try {
				if (reg_str[0] == 'x')
					reg = parseUlong(reg_str.substr(1));
				else
					reg = parseUlong(reg_str);
			} catch (const std::invalid_argument &) {
				if (registers.count(reg_str) != 0)
					reg = registers.at(reg_str);
			}

			if (32 <= reg) {
				console.append("Invalid register: " + reg_str);
				return;
			}

			const Word value = cpu->getRegister(reg);
			console.append(reg_str + " => " + std::to_string(value) + " / " + toHex(value));
		});

		console.addCommand("write", [this](Console &console, auto &pieces) {
			if (!cpu) {
				console.append("No active CPU.");
				return;
			}

			if (pieces.size() != 4) {
				console.append("Usage: write <address> <length> <value>");
				return;
			}

			const auto &address_str = pieces[1], &length_str = pieces[2], &value_str = pieces[3];

			auto try_parse = [&](const Glib::ustring &str, size_t &out, const char *name) -> bool {
				try {
					if (2 < str.size() && str.substr(0, 2) == "0x")
						out = parseUlong(str.substr(2), 16);
					else
						out = parseUlong(str);
				} catch (const std::invalid_argument &) {
					console.append("Couldn't parse " + Glib::ustring(name) + ".");
					return false;
				}
				return true;
			};

			size_t address, length, value;

			if (!try_parse(address_str, address, "address"))
				return;

			if (length_str == "1" || length_str == "b" || length_str == "byte") {
				length = 1;
			} else if (length_str == "2" || length_str == "s" || length_str == "short") {
				length = 2;
			} else if (length_str == "4" || length_str == "d" || length_str == "dword") {
				length = 4;
			} else if (length_str == "8" || length_str == "q" || length_str == "qword") {
				length = 8;
			} else {
				console.append("Couldn't parse length.");
				return;
			}

			if (!try_parse(value_str, value, "value"))
				return;

			if (cpu->memorySize() < address + length) {
				console.append("Can't write out of range.");
				return;
			}

			void *pointer = cpu->getMemory() + address;

			switch (length) {
				case 1: *reinterpret_cast<uint8_t  *>(pointer) = value; break;
				case 2: *reinterpret_cast<uint16_t *>(pointer) = value; break;
				case 4: *reinterpret_cast<uint32_t *>(pointer) = value; break;
				case 8: *reinterpret_cast<uint64_t *>(pointer) = value; break;
			}

			console.append("Wrote value " + toHex(value) + " with size " + std::to_string(length) + " to address "
				+ toHex(address) + ".");
			hexView.update();
		});
	}
}
