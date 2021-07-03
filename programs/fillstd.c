#include <stdlib.h>
#include <string.h>

void main() {
    unsigned char *base = (unsigned char *) 0x80000000;
    memset(base, 0xFF, 360 * 480 * 3);
    while(1);
}