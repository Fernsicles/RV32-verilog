__asm__("li sp, 0x10000000");
// __asm__("a: jal zero, a");
void main() {
    unsigned char *rbase = (unsigned char *) 0x80000000;
	unsigned char *gbase = (unsigned char *) 0x80000000 + 360 * 480;
	unsigned char *bbase = (unsigned char *) 0x80000000 + 360 * 480 * 2;
	unsigned char *ibase = (unsigned char *) 0x100;
	unsigned int *tbase = (unsigned int *) 0x0;
	// for(int x = 0; x < 6572; x++) {
	// 	int y = 0;
	// 	for(int i = 0; i < 360 * 480; i++) {
	// 		unsigned char pixel = ibase[x * 360 * 480];
	// 		rbase[i] = pixel;
	// 		gbase[i] = pixel;
	// 		bbase[i] = pixel;
	// 		// for(int z = 0; z < 8; z++) {
	// 		// 	if(0x1 & pixel) {
	// 		// 		rbase[i * 8 + z] = 0xFF;
	// 		// 		gbase[i * 8 + z] = 0xFF;
	// 		// 		bbase[i * 8 + z] = 0xFF;
	// 		// 	} else {
	// 		// 		rbase[i * 8 + z] = 0x00;
	// 		// 		gbase[i * 8 + z] = 0x00;
	// 		// 		bbase[i * 8 + z] = 0x00;
	// 		// 	}
	// 		// 	pixel = pixel >> z;
	// 		// }
	// 	}
	// }
	int i = 0;
	for(unsigned int x = 0; x < 1135641600; x++) {
		unsigned char pixel = ibase[x];
		rbase[i] = pixel;
		gbase[i] = pixel;
		bbase[i] = pixel;
		if(i >= 360 * 480) {
			i = 0;
		} else {
			i++;
		}
	}
    while(1);
}
