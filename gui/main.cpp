#include <gtkmm.h>
#include <iostream>

#include "App.h"

int main(int argc, char **argv) {
	Glib::RefPtr<RVGUI::App> app = RVGUI::App::create();
	return app->run(argc, argv);
}
