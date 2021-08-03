#include <iostream>

#include "App.h"
#include "CPU.h"
#include "ui/MainWindow.h"

namespace RVGUI {
	MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder_):
	Gtk::ApplicationWindow(cobject), builder(builder_) {
		header = builder->get_widget<Gtk::HeaderBar>("headerbar");
		set_titlebar(*header);

		cssProvider = Gtk::CssProvider::create();
		cssProvider->load_from_resource("/com/ferns/rvgui/style.css");
		Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), cssProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		add_action("open", Gio::ActionMap::ActivateSlot([this] {
			std::cout << "open\n";
			cpu = std::make_shared<CPU>(CPU::Options("./programs/videostd.bin", INT_MAX));
			hexView.setCPU(cpu);
		}));

		functionQueueDispatcher.connect([this] {
			auto lock = std::unique_lock(functionQueueMutex);
			for (auto fn: functionQueue)
				fn();
			functionQueue.clear();
		});

		drawingArea.set_draw_func([this](const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
			if (pixbuf) {
				Gdk::Cairo::set_source_pixbuf(context, pixbuf, 0, 0);
				context->paint();
			}
		});

		paned.set_start_child(drawingArea);
		paned.set_end_child(hexView);
		paned.set_expand(true);
		hexView.set_expand(true);
		delay([this] {
			delay([this] {
				paned.set_position(get_width() * 7 / 10);
			});
		});
		set_child(paned);

		// :(
		g_signal_connect(G_OBJECT(paned.gobj()), "notify::position", G_CALLBACK(
			+[](GtkPaned *, GParamSpec *, gpointer ptr) {
				((HexView *) ptr)->onResize();
			}
		), &hexView);

		// How did I even manage to figure this out? It's poorly documented dark magic as far as I'm concerned.
		g_signal_connect(G_OBJECT(gobj()), "notify::default-height", G_CALLBACK(
			+[](GtkWidget *, GdkEvent *, gpointer ptr) -> gboolean {
				// std::cout << "dh[" << ptr << "]\n";
				((HexView *) ptr)->onResize();
				return false;
			}
		), &hexView);
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
