#pragma once

#include <gtkmm.h>
#include <memory>

namespace RVGUI {
	class CPU;

	class AssemblyView: public Gtk::ScrolledWindow {
		public:
			AssemblyView();

			void setCPU(std::shared_ptr<CPU>);

		private:
			std::shared_ptr<CPU> cpu;
	};
}
