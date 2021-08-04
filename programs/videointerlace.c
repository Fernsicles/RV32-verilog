#include <string.h>
void main() {
    unsigned char *base = (unsigned char *) 0x80000000;
	unsigned char *ibase = (unsigned char *) 0x100;
	unsigned int *tbase = (unsigned int *) 0x0;
	unsigned int t = tbase[0];
	unsigned int now;
	for(unsigned int x = 0; x < 6572; x++) {
		now = tbase[0];
		while(now - t < 34) {
			now = tbase[0];
		}
		t = now;
		for(int i = 0; i < 360 * 480; i++) {
			memset(ibase, *(base + x * 360 * 480 + i), 3);
		}
		ibase += 360 * 480 * 3;
	}
    while(1);
}
