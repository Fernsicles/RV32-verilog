#pragma once

#include <gtkmm.h>
#include <memory>
#include <vector>

#include "Defs.h"

namespace RVGUI {
	class CPU;

	class RegisterView: public Gtk::ScrolledWindow {
		public:
			RegisterView();

			void setCPU(std::shared_ptr<CPU>);
			void update();

		private:
			struct Columns: Gtk::TreeModel::ColumnRecord {
				Columns() {
					add(id);
					add(name);
					add(decimal);
					add(hex);
				}

				Gtk::TreeModelColumn<int> id;
				Gtk::TreeModelColumn<Glib::ustring> name, hex;
				Gtk::TreeModelColumn<Word> decimal;
			};

			static constexpr size_t COUNT = 32;
			static constexpr const char *names[COUNT] = {
				"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
				"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
			};

			std::shared_ptr<CPU> cpu;
			std::vector<Word> lastValues;
			Gtk::TreeView tree;
			Glib::RefPtr<Gtk::ListStore> model;
			std::vector<bool> blank;
			Columns columns;

			int compareNames(const Gtk::TreeModel::const_iterator &, const Gtk::TreeModel::const_iterator &);
			int compareValues(const Gtk::TreeModel::const_iterator &, const Gtk::TreeModel::const_iterator &);
	};
}
