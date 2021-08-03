#include <iostream>

#include "App.h"
#include "ui/MainWindow.h"

namespace RVGUI {
	MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder_):
	Gtk::ApplicationWindow(cobject), builder(builder_) {
		header = builder->get_widget<Gtk::HeaderBar>("headerbar");
		set_titlebar(*header);

		add_action("open", Gio::ActionMap::ActivateSlot([this] {
			std::cout << "open\n";
		}));

		add_action("configure", Gio::ActionMap::ActivateSlot([this] {
			std::cout << "configure\n";
		}));

		functionQueueDispatcher.connect([this] {
			auto lock = std::unique_lock(functionQueueMutex);
			for (auto fn: functionQueue)
				fn();
			functionQueue.clear();
		});

		signal_hide().connect([this] {
			
		});
	}

	MainWindow * MainWindow::create() {
		auto builder = Gtk::Builder::create_from_resource("/com/ferns/rvgui/window.ui");
		auto window = Gtk::Builder::get_widget_derived<MainWindow>(builder, "rvgui_window");
		if (!window)
			throw std::runtime_error("No \"rvgui_window\" object in window.ui");
		return window;
	}

	void MainWindow::delay(std::function<void()> fn) {
		add_tick_callback([fn](const auto &) {
			fn();
			return false;
		});
	}

	void MainWindow::queue(std::function<void()> fn) {
		{
			auto lock = std::unique_lock(functionQueueMutex);
			functionQueue.push_back(fn);
		}
		functionQueueDispatcher.emit();
	}

	void MainWindow::alert(const Glib::ustring &message, Gtk::MessageType type, bool modal, bool use_markup) {
		dialog.reset(new Gtk::MessageDialog(*this, message, use_markup, type, Gtk::ButtonsType::OK, modal));
		dialog->signal_response().connect([this](int) {
			dialog->close();
		});
		dialog->show();
	}

	void MainWindow::error(const Glib::ustring &message, bool modal, bool use_markup) {
		alert(message, Gtk::MessageType::ERROR, modal, use_markup);
	}
}
