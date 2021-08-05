#include <iostream>

#include "ui/Console.h"

namespace RVGUI {
	Console::Console(): Gtk::Box(Gtk::Orientation::VERTICAL) {
		add_css_class("console");
		output.set_expand(true);
		output.add_css_class("output");
		scrolled.set_child(output);
		input.signal_activate().connect(sigc::mem_fun(*this, &Console::onSubmit));
		input.add_css_class("input");
		append(scrolled);
		append(input);
	}

	Console & Console::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		return *this;
	}

	void Console::append(const Glib::ustring &text, bool is_markup) {
		auto &buffer = *output.get_buffer();
		if (buffer.size() != 0)
			buffer.insert(buffer.end(), "\n");
		if (is_markup)
			buffer.insert_markup(buffer.end(), text);
		else
			buffer.insert(buffer.end(), text);
	}

	void Console::onSubmit() {
		Glib::ustring text = input.get_text();
		if (text.empty())
			return;
		input.set_text("");
		append("Invalid command.");
	}
}
