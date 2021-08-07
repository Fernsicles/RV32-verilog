#include "CPU.h"
#include "Util.h"
#include "ui/RegisterView.h"
#include "ui/Util.h"

namespace RVGUI {
	RegisterView::RegisterView(): Gtk::ScrolledWindow() {
		add_css_class("registerview");
		model = Gtk::ListStore::create(columns);
		tree.set_model(model);
		lastValues.reserve(COUNT);
		blank.reserve(COUNT);
		for (size_t i = 0; i < COUNT; ++i) {
			auto &row = *model->append();
			row[columns.id] = i;
			row[columns.name] = names[i];
			row[columns.decimal] = 0;
			row[columns.hex] = "";
			lastValues.push_back(0xdeadbeef);
			blank.push_back(true);
		}
		appendColumn(tree, "Register", columns.name);
		appendColumn(tree, "Decimal", columns.decimal);
		appendColumn(tree, "Hexadecimal", columns.hex);
		model->set_sort_func(columns.name, sigc::mem_fun(*this, &RegisterView::compareNames));
		model->set_sort_func(columns.hex, sigc::mem_fun(*this, &RegisterView::compareValues));
		set_child(tree);
	}

	void RegisterView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
	}

	void RegisterView::update() {
		if (!cpu)
			return;
		for (size_t i = 0; i < COUNT; ++i) {
			const Word new_value = cpu->getRegister(i);
			if (new_value != lastValues[i] || blank[i]) {
				auto iter = model->get_iter(Gtk::TreeModel::Path(1, i));
				lastValues[i] = new_value;
				blank[i] = false;
				(*iter)[columns.decimal] = new_value;
				(*iter)[columns.hex] = toHex(new_value);
			}
		}
	}

	int RegisterView::compareNames(const Gtk::TreeModel::const_iterator &l, const Gtk::TreeModel::const_iterator &r) {
		const int left_id = (*l)[columns.id], right_id = (*r)[columns.id];
		if (left_id < right_id)
			return -1;
		if (left_id == right_id)
			return 0;
		return 1;
	}

	int RegisterView::compareValues(const Gtk::TreeModel::const_iterator &l, const Gtk::TreeModel::const_iterator &r) {
		const Word left_dec = (*l)[columns.decimal], right_dec = (*r)[columns.id];
		if (left_dec < right_dec)
			return -1;
		if (left_dec == right_dec)
			return 0;
		return 1;
	}
}
