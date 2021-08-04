#include <sstream>
#include <stdexcept>

#include "Util.h"

namespace RVGUI {
	unsigned long parseUlong(const std::string &str, int base) {
		const char *c_str = str.c_str();
		char *end;
		unsigned long parsed = strtoul(c_str, &end, base);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not an integer: \"" + str + "\"");
		return parsed;
	}

	std::string toHex(size_t n) {
		std::stringstream ss;
		ss << "0x" << std::hex << n;
		return ss.str();
	}

	std::string toHex(const void *ptr) {
		std::stringstream ss;
		ss << ptr;
		return ss.str();
	}
}