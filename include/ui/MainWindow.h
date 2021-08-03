#pragma once

#include <gtkmm.h>
#include <functional>
#include <list>
#include <mutex>

namespace RVGUI {
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

		private:
			Glib::RefPtr<Gtk::Builder> builder;
			std::unique_ptr<Gtk::Dialog> dialog;
			std::list<std::function<void()>> functionQueue;
			std::mutex functionQueueMutex;
			Glib::Dispatcher functionQueueDispatcher;
	};
}
