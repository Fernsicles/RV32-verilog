#include "ui/Util.h"

namespace RVGUI {
	Gtk::TreeViewColumn * appendColumn(Gtk::TreeView &tree_view, const Glib::ustring &title,
	                                   const Gtk::TreeModelColumn<double> &model_column) {
		Gtk::TreeViewColumn *column = tree_view.get_column(tree_view.append_column_numeric(title, model_column, "%g")
		                                                   - 1);
		column->set_sort_column(model_column);
		return column;
	}
}
