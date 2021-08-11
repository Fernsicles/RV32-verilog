void main() {
	int x = 65362;
	int y = x % 10;
	*(volatile int *) 0 = y;
	for (;;);
}