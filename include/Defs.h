#pragma once

#include <cstdint>
#include <map>

namespace RVGUI {
	using Word = uint32_t;
	enum class VideoMode {Invalid, RGB, Grayscale};

	extern std::map<VideoMode, const char *> videoModeNames;
}
