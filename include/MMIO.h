#include <cstddef>

namespace RVGUI {
	constexpr ptrdiff_t FRAMEBUFFER_READY = 0x00;
	constexpr ptrdiff_t PUTCHAR = 0x01;
	constexpr ptrdiff_t KEYVAL = 0x02;
	constexpr ptrdiff_t KEYPAUSE = 0x04;
	constexpr ptrdiff_t MMIO_END = 0x05;
}
