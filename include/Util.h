#pragma once

#include <string>

namespace RVGUI {
	template <typename T>
	inline T updiv(T n, T d) {
		return n / d + (n % d? 1 : 0);
	}

	unsigned long parseUlong(const std::string &, int base = 10);
	std::string toHex(size_t);
	std::string toHex(const void *);
}
