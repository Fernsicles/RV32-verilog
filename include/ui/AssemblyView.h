#pragma once

#include <gtkmm.h>
#include <memory>
#include <vector>

namespace RVGUI {
	class CPU;

	class AssemblyView: public Gtk::ScrolledWindow {
		public:
			AssemblyView();

			void setCPU(std::shared_ptr<CPU>);

		private:
			Gtk::Grid grid;
			std::shared_ptr<CPU> cpu;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;

			void reset();
	};
}
