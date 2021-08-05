#include <iostream>
#include <regex>

#include "App.h"
#include "CPU.h"
#include "lib/Disassembler.h"
#include "ui/MainWindow.h"
#include "ui/OpenDialog.h"

namespace RVGUI {
	MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder_):
	Gtk::ApplicationWindow(cobject), builder(builder_), hexView(*this), assemblyView(*this) {
		header = builder->get_widget<Gtk::HeaderBar>("headerbar");
		set_titlebar(*header);

		playButton.set_icon_name("media-playback-start-symbolic");
		playButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::play));
		header->pack_start(playButton);

		tickButton.set_icon_name("media-seek-forward-symbolic");
		tickButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::tick));
		header->pack_start(tickButton);

		cssProvider = Gtk::CssProvider::create();
		cssProvider->load_from_resource("/com/ferns/rvgui/style.css");
		Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), cssProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		add_action("open", Gio::ActionMap::ActivateSlot([this] {
			auto *open_dialog = new OpenDialog(*this);
			dialog.reset(open_dialog);
			open_dialog->signal_submit().connect([this](const CPU::Options &options) {
				cpu = std::make_shared<CPU>(options);
				hexView.setCPU(cpu);
				assemblyView.setCPU(cpu);
				if (options.width != 0 && options.height != 0) {
					cairoSurfaceCobj = cairo_image_surface_create_for_data(cpu->getFramebuffer(), CAIRO_FORMAT_A8,
						options.width, options.height, options.width);
					cairoSurface = Cairo::make_refptr_for_instance(new Cairo::Surface(cairoSurfaceCobj));
				}
				drawingArea.queue_draw();
			});
			dialog->show();
		}));

		add_action("play", Gio::ActionMap::ActivateSlot(sigc::mem_fun(*this, &MainWindow::play)));

		functionQueueDispatcher.connect([this] {
			auto lock = std::unique_lock(functionQueueMutex);
			for (auto fn: functionQueue)
				fn();
			functionQueue.clear();
		});

		drawingArea.set_draw_func([this](const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
			if (!cairoSurfaceCobj)
				return;

			if (!cairoPattern) {
				cairoPattern = Cairo::make_refptr_for_instance(
					new Cairo::Pattern(cairo_pattern_create_for_surface(cairoSurfaceCobj), false));
				context->set_source(cairoSurface, 0, 0);
				context->rectangle(0, 0, cpu->getOptions().width, cpu->getOptions().height);
				context->set_source_rgb(0, 0, 0);
			}

			context->set_operator(Cairo::Context::Operator::XOR);
			context->mask(cairoPattern);
			cairoSurface->flush();
			context->paint();
		});

		hpaned.set_start_child(drawingArea);
		hpaned.set_end_child(vpaned);
		hpaned.set_expand(true);
		vpaned.set_expand(true);
		vpaned.set_orientation(Gtk::Orientation::VERTICAL);
		vpaned.set_start_child(hexView);
		vpaned.set_end_child(assemblyView);
		vpaned.set_wide_handle(true);
		delay([this] {
			delay([this] {
				hpaned.set_position(get_width() * 7 / 10);
			});
		});
		set_child(hpaned);

		// :(
		g_signal_connect(G_OBJECT(hpaned.gobj()), "notify::position", G_CALLBACK(
			+[](GtkPaned *, GParamSpec *, gpointer ptr) {
				((HexView *) ptr)->onResize();
			}
		), &hexView);

		// How did I even manage to figure this out? It's poorly documented dark magic as far as I'm concerned.
		g_signal_connect(G_OBJECT(gobj()), "notify::default-height", G_CALLBACK(
			+[](GtkWidget *, GdkEvent *, gpointer ptr) -> gboolean {
				((HexView *) ptr)->onResize();
				return false;
			}
		), &hexView);

		signal_hide().connect([this] {
			playing = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

	void MainWindow::onUpdatePC(uint32_t pc) {
		hexView.updatePC(pc);
		assemblyView.updatePC(pc);
	}

	void MainWindow::play() {
		if (cpu && !playing) {
			playing = true;
			playThread = std::thread([this] {
				while (playing)
					cpu->tick();
			});
			playThread.detach();
			timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::onTimeout), 1000 / FPS);
		} else {
			playing = false;
			if (cpu) {
				hexView.updatePC(cpu->getPC());
				assemblyView.updatePC(cpu->getPC());
			}
		}
		playButton.set_active(playing);
	}

	void MainWindow::tick() {
		if (cpu) {
			cpu->tick();
			drawingArea.queue_draw();
			hexView.updatePC(cpu->getPC());
			assemblyView.updatePC(cpu->getPC());
		}
	}

	bool MainWindow::onTimeout() {
		if (cpu) {
			drawingArea.queue_draw();
			hexView.update();
			hexView.updatePC(cpu->getPC());
			assemblyView.updatePC(cpu->getPC());
		}

		return playing;
	}
}
