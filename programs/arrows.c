#include "mmio.h"

void puts(const char *str);

void main() {
	for (;;) {
		const int keyval = *KEYVALUE;
		if (!keyval)
			continue;
		if (keyval == 65362) { // up
			puts("\x1b[A");
		} else if (keyval == 65364) { // down
			puts("\x1b[B");
		} else if (keyval == 65363) { // right
			puts("\x1b[C");
		} else if (keyval == 65361) { // left
			puts("\x1b[D");
		} else if (keyval < 256) {
			puts("\x1b[s");
			*PUTCHAR = keyval;
			puts("\x1b[u");
		}
	}
}

void puts(const char *str) {
	for (int i = 0; str[i] != '\0'; ++i)
		*PUTCHAR = str[i];
}
