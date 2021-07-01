__asm__("li sp, 0x100");
void main() {
    unsigned char *base = (unsigned char *) 0x80000000;
    for(int i = 0; i < (360 * 480 * 3) / 4; i++) {
        ((unsigned int *) base)[i] = 0xFFFFFFFF;
    }
    while(1);
}