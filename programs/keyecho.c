int main() {
	volatile unsigned char *putchar = (volatile unsigned char *) 0x80000001;
	volatile unsigned char *keyval  = (volatile unsigned char *) 0x80000002;
	for (;;)
		*putchar = *keyval;
}