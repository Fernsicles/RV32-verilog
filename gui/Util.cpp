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
}