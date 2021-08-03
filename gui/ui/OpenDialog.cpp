#include "ui/OpenDialog.h"

namespace RVGUI {
	OpenDialog::OpenDialog(Gtk::Window &parent, bool modal): Gtk::Dialog("Open", parent, modal) {
		set_default_size(400, -1);
		auto &area = *get_content_area();
		area.set_orientation(Gtk::Orientation::VERTICAL);
		area.set_margin(5);
		area.set_spacing(5);

		programBox.append(programFilename);
		programBrowse.set_icon_name("document-open-symbolic");
		programBox.append(programBrowse);
		programFilename.set_hexpand(true);
		programFilename.set_placeholder_text("Program");
		programBox.add_css_class("linked");

		dataBox.append(dataFilename);
		dataBrowse.set_icon_name("document-open-symbolic");
		dataBox.append(dataBrowse);
		dataFilename.set_hexpand(true);
		dataFilename.set_placeholder_text("Data");
		dataBox.add_css_class("linked");

		memorySize.set_placeholder_text("Memory Size");

		width.set_placeholder_text("Width");
		width.set_text("480");
		width.set_hexpand(true);
		height.set_placeholder_text("Height");
		height.set_text("360");
		height.set_hexpand(true);
		dimensionsBox.set_spacing(5);
		dimensionsBox.append(width);
		dimensionsBox.append(height);

		mmioOffset.set_placeholder_text("MMIO Offset");
		dataOffset.set_placeholder_text("Data Offset");
		timeOffset.set_placeholder_text("Time Offset");

		separateInstructions.set_label("Separate Instructions");

		buttonBox.set_halign(Gtk::Align::END);
		buttonBox.set_spacing(5);
		buttonBox.append(cancelButton);
		buttonBox.append(clearButton);
		buttonBox.append(okButton);

		area.append(programBox);
		area.append(dataBox);
		area.append(memorySize);
		area.append(dimensionsBox);
		area.append(mmioOffset);
		area.append(dataOffset);
		area.append(timeOffset);
		area.append(separateInstructions);
		area.append(buttonBox);

		clearButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::clear));
		programBrowse.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::browseProgram));
		dataBrowse.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::browseData));
		cancelButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::hide));
		okButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::submit));
		for (auto *entry: {&programFilename, &dataFilename, &width, &height, &mmioOffset, &dataOffset, &timeOffset,
		                   &memorySize})
			entry->signal_activate().connect(sigc::mem_fun(*this, &OpenDialog::submit));
	}

	void OpenDialog::submit() {

	}

	void OpenDialog::clear() {
		programFilename.set_text("");
		dataFilename.set_text("");
		memorySize.set_text("");
		width.set_text("");
		height.set_text("");
		mmioOffset.set_text("");
		dataOffset.set_text("");
		timeOffset.set_text("");
		separateInstructions.set_active(false);
	}

	Gtk::FileChooserDialog * OpenDialog::browse(const Glib::ustring &title,
	                                            std::function<void(const std::string &)> on_choose) {
		auto *browser = new Gtk::FileChooserDialog(*this, title, Gtk::FileChooser::Action::OPEN, true);
		browser->set_transient_for(*this);
		browser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
		browser->add_button("_Open", Gtk::ResponseType::OK);
		browser->signal_response().connect([this, browser, on_choose](int response) {
			if (response == Gtk::ResponseType::OK)
				on_choose(browser->get_file()->get_path());
			else
				on_choose("");
			browser->hide();
		});
		browser->show();
		return browser;
	}

	void OpenDialog::browseProgram() {
		dialog.reset(browse("Program", [this](const std::string &path) {
			programFilename.set_text(path);
		}));
	}

	void OpenDialog::browseData() {
		dialog.reset(browse("Data", [this](const std::string &path) {
			dataFilename.set_text(path);
		}));
	}
}
