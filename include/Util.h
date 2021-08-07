#pragma once

#include <gtkmm.h>
#include <string>
#include <vector>

namespace RVGUI {
	template <typename T>
	inline T updiv(T n, T d) {
		return n / d + (n % d? 1 : 0);
	}

	/** Splits a string by a given delimiter. If condense is true, empty strings won't be included in the output. */
	template <typename T>
	std::vector<T> split(const T &str, const T &delimiter, bool condense = true) {
		if (str.empty())
			return {};

		size_t next = str.find(delimiter);
		if (next == T::npos)
			return {str};

		std::vector<T> out {};
		const size_t delimiter_length = delimiter.size();
		size_t last = 0;

		out.push_back(str.substr(0, next));

		while (next != T::npos) {
			last = next;
			next = str.find(delimiter, last + delimiter_length);
			T sub = str.substr(last + delimiter_length, next - last - delimiter_length);
			if (!sub.empty() || !condense)
				out.push_back(std::move(sub));
		}

		return out;
	}

	template <typename Iter>
	Glib::ustring join(Iter begin, Iter end, const Glib::ustring &delim = " ") {
		if (begin == end)
			return "";
		std::ostringstream oss;
		bool first = true;
		while (begin != end) {
			if (!first)
				oss << delim;
			else
				first = false;
			oss << *begin;
			++begin;
		}
		return oss.str();
	}

	template <typename Container>
	Glib::ustring join(const Container &cont, const Glib::ustring &delim = " ") {
		return join(cont.begin(), cont.end(), delim);
	}

	template <typename T>
	std::vector<T> split(const T &str, const char *delimiter, bool condense = true) {
		return split(str, T(delimiter), condense);
	}

	unsigned long parseUlong(const std::string &, int base = 10);
	long parseLong(const std::string &, int base = 10);
	std::string toHex(size_t);
	std::string toHex(const void *);
}
