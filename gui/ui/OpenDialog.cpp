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
	}
}
