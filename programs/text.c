void main() {
	volatile unsigned char *putchar_cell = (volatile unsigned char *) 0x80000001;
	for (;;) {
		*putchar_cell = 'A';
		*putchar_cell = 'B';
		*putchar_cell = 'C';
		*putchar_cell = 'D';
		*putchar_cell = 'E';
		*putchar_cell = 'F';
		*putchar_cell = 'G';
	}
}