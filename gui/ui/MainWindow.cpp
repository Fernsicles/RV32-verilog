#include <iostream>
#include <regex>

#include "App.h"
#include "CPU.h"
#include "MMIO.h"
#include "lib/Disassembler.h"
#include "ui/MainWindow.h"
#include "ui/OpenDialog.h"

namespace RVGUI {
	MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder_):
	Gtk::ApplicationWindow(cobject), builder(builder_), hexView(*this), assemblyView(*this), registerView(*this) {
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

		add_action("open", Gio::ActionMap::ActivateSlot(sigc::mem_fun(*this, &MainWindow::open)));
		add_action("play", Gio::ActionMap::ActivateSlot(sigc::mem_fun(*this, &MainWindow::play)));

		functionQueueDispatcher.connect([this] {
			auto lock = std::unique_lock(functionQueueMutex);
			for (auto fn: functionQueue)
				fn();
			functionQueue.clear();
		});

		terminal = Gtk::manage(Glib::wrap(GTK_WIDGET(vte_terminal_new()), false));
		terminal->set_expand(true);
		vte = VTE_TERMINAL(terminal->gobj());

		hpaned.set_start_child(vpanedLeft);
		hpaned.set_end_child(vpanedRight);
		hpaned.set_expand(true);

		vpanedLeft.set_expand(true);
		vpanedLeft.set_orientation(Gtk::Orientation::VERTICAL);
		vpanedLeft.set_start_child(centerView);
		vpanedLeft.set_end_child(console);
		vpanedLeft.set_wide_handle(true);

		notebookTop.set_group_name("rvgui");
		notebookTop.append_page(hexView, "Hex View");
		notebookTop.set_tab_detachable(hexView);
		notebookTop.set_tab_reorderable(hexView);
		notebookBottom.set_group_name("rvgui");
		notebookBottom.append_page(assemblyView, "Assembly");
		notebookBottom.set_tab_detachable(assemblyView);
		notebookBottom.set_tab_reorderable(assemblyView);
		notebookBottom.append_page(registerView, "Registers");
		notebookBottom.set_tab_detachable(registerView);
		notebookBottom.set_tab_reorderable(registerView);

		auto click = Gtk::GestureClick::create();
		click->signal_released().connect([this](int count, double, double) { if (count == 2) registerView.update(); });
		notebookBottom.get_tab_label(registerView)->add_controller(click);

		auto key_controller = Gtk::EventControllerKey::create();
		key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::onKeyPressed), true);
		terminal->add_controller(key_controller);

		vpanedRight.set_expand(true);
		vpanedRight.set_orientation(Gtk::Orientation::VERTICAL);
		vpanedRight.set_start_child(notebookTop);
		vpanedRight.set_end_child(notebookBottom);
		vpanedRight.set_wide_handle(true);

		delay([this] {
			hpaned.set_position(get_width() * 7 / 10);
			vpanedLeft.set_position(get_height() * 7 / 10);
		}, 2);

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

		addCommands();
	}

	MainWindow * MainWindow::create() {
		auto builder = Gtk::Builder::create_from_resource("/com/ferns/rvgui/window.ui");
		auto window = Gtk::Builder::get_widget_derived<MainWindow>(builder, "rvgui_window");
		if (!window)
			throw std::runtime_error("No \"rvgui_window\" object in window.ui");
		return window;
	}

	void MainWindow::delay(std::function<void()> fn, unsigned count) {
		if (count <= 1)
			add_tick_callback([fn](const auto &) {
				fn();
				return false;
			});
		else
			delay([this, fn, count] {
				delay(fn, count - 1);
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

	void MainWindow::open() {
		auto *open_dialog = new OpenDialog(*this);
		dialog.reset(open_dialog);
		open_dialog->signal_submit().connect([this](const CPU::Options &options) {
			stop();
			cpu = std::make_shared<CPU>(options);
			centerView.setDimensions(options.width, options.height);
			if (!initVideo(*cpu))
				cpu.reset();
			hexView.setCPU(cpu);
			assemblyView.setCPU(cpu);
			registerView.setCPU(cpu);
			if (cpu)
				cpu->onPrint = [this](char ch) {
					// Not thread safe and kinda crashy if you keep toggling play. I'd use the queue function,
					// but the dispatcher gets stuck in write() pretty quickly.
					vte_terminal_feed(vte, &ch, 1);
				};
			drawingArea.queue_draw();
		});
		dialog->show();
	}

	void MainWindow::play() {
		if (cpu && !playing) {
			startPlayThread();
		} else
			stop();
		playButton.set_active(playing);
	}

	void MainWindow::startPlayThread() {
		playing = true;
		playThread = std::thread([this] {
			try {
				while (playing)
					switch (cpu->tick()) {
						case CPU::TickResult::Continue:
							break;
						case CPU::TickResult::KeyPause:
							keyPause = true;
						case CPU::TickResult::Finished:
							playing = false;
							break;
					}
			} catch (const std::exception &err) {
				Glib::ustring what = err.what();
				queue([this, what] {
					stop();
					error("Error while simulating: " + what);
				});
			}
		});
		playThread.detach();
		timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::onTimeout), 1000 / FPS);
	}

	void MainWindow::stop() {
		playing = false;
		playButton.set_active(false);
		if (cpu) {
			hexView.updatePC(cpu->getPC());
			assemblyView.updatePC(cpu->getPC());
		}
	}

	void MainWindow::tick() {
		if (cpu && !playing) {
			try {
				cpu->tick();
			} catch (const std::exception &err) {
				error("Error while simulating: " + Glib::ustring(err.what()));
				return;
			}

			drawingArea.queue_draw();
			hexView.updatePC(cpu->getPC());
			assemblyView.updatePC(cpu->getPC());
			registerView.update();
		}
	}

	bool MainWindow::onTimeout() {
		if (cpu) {
			drawingArea.queue_draw();
			hexView.update();
			hexView.updatePC(cpu->getPC());
			assemblyView.updatePC(cpu->getPC());
			registerView.update();
		}

		return playing;
	}

	bool MainWindow::initVideo(const CPU &cpu) {
		const auto &options = cpu.getOptions();
		if (options.width != 0 && options.height != 0) {
			cairoSurfaceCobj = nullptr;
			cairoSurface.reset();
			cairoPattern.reset();
			pixbuf.reset();
			uint8_t *framebuffer = cpu.getFramebuffer();
			vte_terminal_reset(vte, true, true);
			switch (videoMode = options.videoMode) {
				case VideoMode::Grayscale:
					if (options.width % sizeof(uint32_t)) {
						delay([this] {
							error("Framebuffer width must be a multiple of " + std::to_string(sizeof(uint32_t)));
						});
						return false;
					}
					centerView.setChild(drawingArea);
					cairoSurfaceCobj = cairo_image_surface_create_for_data(framebuffer, CAIRO_FORMAT_A8, options.width,
						options.height, options.width);
					cairoSurface = Cairo::make_refptr_for_instance(new Cairo::Surface(cairoSurfaceCobj));
					drawingArea.set_draw_func(sigc::mem_fun(*this, &MainWindow::drawGrayscale));
					break;
				case VideoMode::RGB:
					centerView.setChild(drawingArea);
					pixbuf = Gdk::Pixbuf::create_from_data(framebuffer, Gdk::Colorspace::RGB, false, 8, options.width,
						options.height, 3 * options.width);
					drawingArea.set_draw_func(sigc::mem_fun(*this, &MainWindow::drawRGB));
					break;
				case VideoMode::Text:
					vte_terminal_set_size(vte, options.width, options.height);
					centerView.setChild(*terminal);
					delay([this] { centerView.updateChild(); }, 2); // :(
					break;
				default:
					throw std::runtime_error("Invalid VideoMode: "
						+ std::to_string(static_cast<int>(options.videoMode)));
			}
		}
		return true;
	}

	void MainWindow::drawGrayscale(const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
		if (!cairoSurfaceCobj)
			return;

		if (!cairoPattern) {
			cairoPattern = Cairo::make_refptr_for_instance(
				new Cairo::Pattern(cairo_pattern_create_for_surface(cairoSurfaceCobj), false));
			context->set_source(cairoSurface, 0, 0);
			context->rectangle(0, 0, cpu->getOptions().width, cpu->getOptions().height);
			context->set_source_rgb(0, 0, 0);
		}

		if (cpu) {
			uint8_t &ready = cpu->framebufferReady;
			while (!ready);
			context->set_operator(Cairo::Context::Operator::XOR);
			context->mask(cairoPattern);
			context->paint();
			if (ready == 1)
				ready = 0;
		}
	}

	void MainWindow::drawRGB(const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
		if (pixbuf && cpu) {
			uint8_t &ready = cpu->framebufferReady;
			while (!ready);
			Gdk::Cairo::set_source_pixbuf(context, pixbuf, 0, 0);
			context->paint();
			if (ready == 1)
				ready = 0;
		}
	}

	bool MainWindow::onKeyPressed(guint keyval, guint, Gdk::ModifierType) {
		if (cpu) {
			cpu->lastKeyValue = keyval;
			if (keyPause) {
				keyPause = false;
				startPlayThread();
			}
		}
		return true;
	}
}
