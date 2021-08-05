#include <iostream>
#include <filesystem>

#include "ui/OpenDialog.h"
#include "Util.h"

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

		widthEntry.set_placeholder_text("Width");
		widthEntry.set_text("480");
		widthEntry.set_hexpand(true);
		heightEntry.set_placeholder_text("Height");
		heightEntry.set_text("360");
		heightEntry.set_hexpand(true);
		dimensionsBox.set_spacing(5);
		dimensionsBox.append(widthEntry);
		dimensionsBox.append(heightEntry);

		mmioOffset.set_placeholder_text("MMIO Offset");
		dataOffset.set_placeholder_text("Data Offset");
		timeOffset.set_placeholder_text("Time Offset");

		separateInstructions.set_label("Separate Instructions");

		buttonBox.set_halign(Gtk::Align::END);
		buttonBox.set_spacing(5);
		buttonBox.append(cancelButton);
		buttonBox.append(clearButton);
		buttonBox.append(okButton);

		videoModeModel = Gtk::ListStore::create(videoModeColumns);
		videoModeCombo.set_model(videoModeModel);
		resetVideoModelCombo();
		videoModeCombo.pack_start(videoModeColumns.name);
		videoModeCombo.signal_changed().connect(sigc::mem_fun(*this, &OpenDialog::videoModelChanged));

		area.append(programBox);
		area.append(dataBox);
		area.append(memorySize);
		area.append(dimensionsBox);
		area.append(mmioOffset);
		area.append(dataOffset);
		area.append(timeOffset);
		area.append(separateInstructions);
		area.append(videoModeCombo);
		area.append(buttonBox);

		setDefaults();

		clearButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::clear));
		programBrowse.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::browseProgram));
		dataBrowse.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::browseData));
		cancelButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::hide));
		okButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenDialog::submit));
		for (auto *entry : {&programFilename, &dataFilename, &widthEntry, &heightEntry, &mmioOffset, &dataOffset,
				 &timeOffset, &memorySize})
			entry->signal_activate().connect(sigc::mem_fun(*this, &OpenDialog::submit));
	}

	void OpenDialog::submit() {
		const auto program	   = programFilename.get_text();
		const auto memsize_str = memorySize.get_text();

		if (program.empty()) {
			error("Please specify a program filename.");
			return;
		}

		if (memsize_str.empty()) {
			error("Please specify a memory size.");
			return;
		}

		size_t memsize;
		try {
			memsize = parseUlong(memsize_str);
		} catch (const std::invalid_argument &) {
			error("Invalid memory size.");
			return;
		}

		size_t data_offset = 0;
		try {
			if (dataOffset.get_text_length() != 0) {
				data_offset = parseUlong(dataOffset.get_text());
				if (UINT_MAX < data_offset)
					throw std::invalid_argument("Invalid data offset.");
			}
		} catch (const std::invalid_argument &) {
			error("Invalid data offset.");
			return;
		}

		size_t time_offset = 0;
		bool use_time_offset = false;
		try {
			if (timeOffset.get_text_length() != 0) {
				time_offset = parseUlong(timeOffset.get_text());
				if (INT_MAX < time_offset)
					throw std::invalid_argument("Invalid time offset.");
				use_time_offset = true;
			}
		} catch (const std::invalid_argument &) {
			error("Invalid time offset.");
			return;
		}

		size_t mmio_offset = 0x80'00'00'00;
		try {
			if (mmioOffset.get_text_length() != 0) {
				mmio_offset = parseUlong(mmioOffset.get_text());
				if (UINT_MAX < mmio_offset)
					throw std::invalid_argument("Invalid MMIO offset.");
			}
		} catch (const std::invalid_argument &) {
			error("Invalid MMIO offset.");
			return;
		}

		size_t width = 0;
		try {
			if (widthEntry.get_text_length() != 0) {
				width = parseUlong(widthEntry.get_text());
				if (UINT_MAX < width)
					throw std::invalid_argument("Invalid width.");
			}
		} catch (const std::invalid_argument &) {
			error("Invalid width.");
			return;
		}

		size_t height = 0;
		try {
			if (heightEntry.get_text_length() != 0) {
				height = parseUlong(heightEntry.get_text());
				if (UINT_MAX < height)
					throw std::invalid_argument("Invalid height.");
			}
		} catch (const std::invalid_argument &) {
			error("Invalid height.");
			return;
		}

		CPU::Options options(program, memsize);
		options.setDataFilename(dataFilename.get_text());
		options.setDataOffset(static_cast<Word>(data_offset));
		options.setSeparateInstructions(separateInstructions.get_active());
		options.setDimensions(width, height);
		options.setMMIOOffset(static_cast<Word>(mmio_offset));
		options.setVideoMode(videoMode);
		std::cout << "videoMode[" << videoModeNames[videoMode] << "]\n";
		if (use_time_offset)
			options.setTimeOffset(static_cast<int32_t>(time_offset));
		hide();
		signal_submit_.emit(options);
	}

	void OpenDialog::clear() {
		programFilename.set_text("");
		dataFilename.set_text("");
		memorySize.set_text("");
		widthEntry.set_text("");
		heightEntry.set_text("");
		mmioOffset.set_text("");
		dataOffset.set_text("");
		timeOffset.set_text("");
		separateInstructions.set_active(false);
	}

	Gtk::FileChooserDialog *OpenDialog::browse(
		const Glib::ustring &title, std::function<void(const std::string &)> on_choose) {
		auto *browser = new Gtk::FileChooserDialog(*this, title, Gtk::FileChooser::Action::OPEN, true);
		browser->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path()));
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
		dialog.reset(browse("Program", [this](const std::string &path) { programFilename.set_text(path); }));
	}

	void OpenDialog::browseData() {
		dialog.reset(browse("Data", [this](const std::string &path) { dataFilename.set_text(path); }));
	}

	void OpenDialog::alert(const Glib::ustring &message, Gtk::MessageType type, bool modal, bool use_markup) {
		dialog.reset(new Gtk::MessageDialog(*this, message, use_markup, type, Gtk::ButtonsType::OK, modal));
		dialog->signal_response().connect([this](int) { dialog->close(); });
		dialog->show();
	}

	void OpenDialog::error(const Glib::ustring &message, bool modal, bool use_markup) {
		alert(message, Gtk::MessageType::ERROR, modal, use_markup);
	}

	void OpenDialog::videoModelChanged() {
		if (const auto iter = videoModeCombo.get_active())
			videoMode = (*iter)[videoModeColumns.mode];
	}

	void OpenDialog::resetVideoModelCombo() {
		videoModeModel->clear();
		for (const auto [mode, name]: videoModeNames) {
			auto iter = videoModeModel->append();
			(*iter)[videoModeColumns.name] = name;
			(*iter)[videoModeColumns.mode] = mode;
			if (mode == videoMode)
				videoModeCombo.set_active(iter);
		}
	}

	void OpenDialog::setDefaults() {
		programFilename.set_text("./programs/videograyscale.bin");
		dataFilename.set_text("./bad-apple.raw");
		memorySize.set_text("2147483647");
		dataOffset.set_text("256");
		timeOffset.set_text("0");
		separateInstructions.set_active(true);
		for (auto &row: videoModeModel->children()) {
			if (row[videoModeColumns.mode] == VideoMode::Grayscale) {
				videoModeCombo.set_active(row.get_iter());
				break;
			}
		}
	}
}
