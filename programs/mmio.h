#ifndef MMIO_H_
#define MMIO_H_

#define FRAMEBUFFER_READY ((volatile unsigned char  *) 0x80000000)
#define PUTCHAR           ((volatile unsigned char  *) 0x80000001)
#define KEYVALUE          ((volatile unsigned short *) 0x80000002)
#define KEYPAUSE          ((volatile unsigned char  *) 0x80000004)
#define FRAMEBUFFER       ((volatile unsigned char  *) 0x81000000)

#endif
