#include <string.h>
void main() {
	volatile unsigned char *readyflag = (volatile unsigned char *) 0x80000000;
	unsigned char *framebuffer = (unsigned char *) 0x81000000;
	unsigned char *ibase = (unsigned char *) 0x100;
	unsigned int *tbase = (unsigned int *) 0x0;
	int i = 0;
	unsigned int t = tbase[0];
	unsigned int now;
	*readyflag = 0;
	for (unsigned int x = 0; x < 6572; x++) {
		now = tbase[0];
		while (now - t < 34 || *readyflag == 1)
			now = tbase[0];
		t = now;
		memcpy(framebuffer, ibase, 360 * 480);
		*readyflag = 1;
		ibase += 360 * 480;
	}
	while(1);
}
