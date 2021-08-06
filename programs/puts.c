void puts(const char *str) {
	for (int i = 0; str[i] != '\0'; ++i)
		*(volatile unsigned char *) 0x80000001 = str[i];
}

void main() {
	for (;;) {
		puts("Hello, World!\n");
	}
}