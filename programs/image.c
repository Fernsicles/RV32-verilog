__asm__("li sp, 0x10000000");
// __asm__("a: jal zero, a");
void main() {
	unsigned char *framebuffer = (unsigned char *) 0x80100000;
    unsigned char *rbase = framebuffer;
	unsigned char *gbase = framebuffer + 360 * 480;
	unsigned char *bbase = framebuffer + 360 * 480 * 2;
	unsigned char *ibase = (unsigned char *) 0x100;
	for(int x = 0; x < 480 * 360; x++) {
		unsigned char pixel = ibase[x];
		rbase[x] = pixel;
		gbase[x] = pixel;
		bbase[x] = pixel;
	}
    while(1);
}
