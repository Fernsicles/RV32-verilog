#include <iostream>

#include "Util.h"
#include "ui/Console.h"

namespace RVGUI {
	Console::Console(): Gtk::Box(Gtk::Orientation::VERTICAL) {
		add_css_class("console");
		output.set_expand(true);
		output.set_editable(false);
		output.add_css_class("output");
		output.set_cursor_visible(false);
		output.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
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

	void Console::addCommand(const Glib::ustring &name, const CommandHandler &handler) {
		if (commands.count(name) != 0)
			throw std::runtime_error("Console already contains a handler for command \"" + name + "\"");
		commands.try_emplace(name, handler);
	}

	void Console::onSubmit() {
		const Glib::ustring text = input.get_text();
		if (text.empty())
			return;
		input.set_text("");

		std::vector<Glib::ustring> pieces = split(text, " ", true);

		if (!pieces.empty() && pieces.front().empty())
			pieces.erase(pieces.begin());

		if (pieces.empty())
			return;

		const Glib::ustring &command = pieces[0];

		if (commands.count(command) == 0)
			append("Invalid command.");
		else
			commands.at(command)(*this, pieces);
	}
}
