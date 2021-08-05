#pragma once

#include <gtkmm.h>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include "Defs.h"
#include "ui/AssemblyView.h"
#include "ui/CenterView.h"
#include "ui/Console.h"
#include "ui/HexView.h"

namespace RVGUI {
	class CPU;

	class MainWindow: public Gtk::ApplicationWindow {
		public:
			Gtk::HeaderBar *header;

			MainWindow(BaseObjectType *, const Glib::RefPtr<Gtk::Builder> &);

			static MainWindow * create();

			/** Causes a function to occur on the next Gtk tick (or possibly later). Not thread-safe. */
			void delay(std::function<void()>);

			/** Queues a function to be executed in the Gtk thread. Thread-safe. Can be used from any thread. */
			void queue(std::function<void()>);

			/** Displays an alert. This will reset the dialog pointer. If you need to use this inside a dialog's code,
			 *  use delay(). */
			void alert(const Glib::ustring &message, Gtk::MessageType = Gtk::MessageType::INFO, bool modal = true,
			           bool use_markup = false);

			/** Displays an error message. (See alert().) */
			void error(const Glib::ustring &message, bool modal = true, bool use_markup = false);

			void onUpdatePC(uint32_t pc);

		private:
			static constexpr int FPS = 60;

			Glib::RefPtr<Gtk::Builder> builder;
			Glib::RefPtr<Gtk::CssProvider> cssProvider;
			std::unique_ptr<Gtk::Dialog> dialog;
			std::list<std::function<void()>> functionQueue;
			std::mutex functionQueueMutex;
			Glib::Dispatcher functionQueueDispatcher;
			std::shared_ptr<CPU> cpu;
			std::shared_ptr<uint8_t[]> framebuffer;
			sigc::connection timeout;
			std::thread playThread;
			VideoMode videoMode = VideoMode::RGB;
			Glib::RefPtr<Gdk::Pixbuf> pixbuf; // For RGB
			cairo_surface_t *cairoSurfaceCobj = nullptr; // For grayscale
			Cairo::RefPtr<Cairo::Surface> cairoSurface;
			Cairo::RefPtr<Cairo::Pattern> cairoPattern;

			Gtk::ToggleButton playButton;
			Gtk::Button tickButton;
			Gtk::Paned hpaned, vpanedLeft, vpanedRight;
			Gtk::DrawingArea drawingArea;
			CenterView centerView;
			HexView hexView;
			AssemblyView assemblyView;
			Console console;

			bool playing = false;

			void open();
			void play();
			void stop();
			void tick();
			bool onTimeout();
			bool initVideo(const CPU &);
			void drawGrayscale(const Cairo::RefPtr<Cairo::Context> &, int width, int height);
			void drawRGB(const Cairo::RefPtr<Cairo::Context> &, int width, int height);

			void addCommands();
	};
}
