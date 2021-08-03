#include "ui/OpenDialog.h"

namespace RVGUI {
	OpenDialog::OpenDialog(Gtk::Window &parent, bool modal): Gtk::Dialog("Open", parent, modal) {
		set_default_size(300, -1);
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

		area.append(programBox);
		area.append(dataBox);
	}
}
