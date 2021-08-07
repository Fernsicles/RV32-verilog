#pragma once

#include <gtkmm.h>

namespace std {
	template <>
	struct hash<Glib::ustring> {
		std::size_t operator()(const Glib::ustring &str) const noexcept {
			return std::hash<std::string>()(str);
		}
	};
}
