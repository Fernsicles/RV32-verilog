__asm__("li sp, 0x10000000");
void main() {
    unsigned char *base = (unsigned char *) 0x80000000;
    for(int i = 0; i < 360 * 480 * 3; i++) {
        base[i] = 0xFF;
    }
    while(1);
}
