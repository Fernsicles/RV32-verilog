#pragma once

#include <gtkmm.h>

namespace RVGUI {
	class CenterView: public Gtk::Fixed {
		public:
			CenterView(Gtk::Widget * = nullptr);
			CenterView(Gtk::Widget *, int width_, int height_);

			void setDimensions(int width_, int height_);
			void setChild(Gtk::Widget &);
			void updateChild();

		private:
			Gtk::Widget *child;
			int width = 0, height = 0;
			int lastWidth = 0, lastHeight = 0;

	};
}
