#pragma once

#include <gtkmm.h>

namespace RVGUI {
	class CenterView: public Gtk::Fixed {
		public:
			CenterView(Gtk::Widget &);
			CenterView(Gtk::Widget &, int width_, int height_);

			void setDimensions(int width_, int height_);

		private:
			Gtk::Widget &child;
			int width = 0, height = 0;
			int lastWidth = 0, lastHeight = 0;

			void updateChild();
	};
}
