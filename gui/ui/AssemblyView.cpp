#include "ui/AssemblyView.h"

namespace RVGUI {
	AssemblyView::AssemblyView(): Gtk::ScrolledWindow() {

	}

	void AssemblyView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
	}
}
