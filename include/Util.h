#pragma once

namespace RVGUI {
	template <typename T>
	inline T updiv(T n, T d) {
		return n / d + (n % d? 1 : 0);
	}
}
