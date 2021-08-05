#include "Util.h"
#include "ui/MainWindow.h"

namespace RVGUI {
	void MainWindow::addCommands() {
		console.addCommand("echo", [](Console &console, auto &pieces) {
			console.append(join(pieces.begin() + 1, pieces.end(), " "));
		});
	}
}
