/*
 *  a2, an Apple II emulator in C
 *  (c) Copyright 1990 by Rich Skrenta
 *
 *  Command line interface written by Tom Markson
 *
 *  Distribution agreement:
 *
 *	You may freely copy or redistribute this software, so long
 *	as there is no profit made from its use, sale, trade or
 *	reproduction.  You may not change this copyright notice,
 *	and it must be included prominently in any copy made.
 *
 *  Send emulator related mail to:  skrenta@blekko.commodore.com
 *				    skrenta@blekko.uucp
 */


#import	<stdio.h>
#import	<libc.h>
#import	"a2.h"


unsigned char mem[65536];		/* 64K memory image */
unsigned char jmp_tbl[65536];		/* jmp & jsr interceptor functions */
void (*mem_set[256])(unsigned short, unsigned char);/* memory store interceptors */

unsigned char rom[0x3000];
unsigned char ram[0x2000];
unsigned char bank1[0x1000];
unsigned char bank2[0x1000];

int screen_page = PRIMARY_PAGE;

int ram_read = 0;
int ram_write = 0;
int bank2_enable = 0;

#ifdef NeXT
#define ram_copy(f,t,l) memcpy(t,f,l)
#else
ram_copy(from, to, len)
unsigned char *from;
unsigned char *to;
int len;
{

	while (len--)
		*to++ = *from++;
}
#endif

/*
 *  Write protect/unprotect D000-FFFF
 */
void writep(void (*fn)(unsigned short, unsigned char))
{
int i;

	for (i = 0xD0; i <= 0xFF; i++)
		mem_set[i] = fn;
}

void set_writep(unsigned short a, unsigned char n)
{
  /* Just eat it. */
}

void set_ramwrite(unsigned short a, unsigned char n)
{

	if (a >= 0xE000)
		ram[a - 0xE000] = n;
	else if (bank2_enable)
		bank2[a - 0xD000] = n;
	else
		bank1[a - 0xD000] = n;
}

void memory_setup(void)
{
long i;

	for (i = 0; i < 256; i++)
		mem_set[i] = NULL;

	for (i = 0; i < 65536; i++)
		jmp_tbl[i] = 0;

	writep(set_writep);
	ram_copy(&mem[0xD000], rom, 0x3000);
}






void ram_card(unsigned short addr)
{

	if (ram_read) {
		ram_copy(&mem[0xE000], ram, 0x2000);
		if (bank2_enable)
			ram_copy(&mem[0xD000], bank2, 0x1000);
		else
			ram_copy(&mem[0xD000], bank1, 0x1000);
	}

	switch (addr & 0x000F) {
	case 0x00: 			/* C080 */
		ram_read  = 1;
		ram_write = 0;
		bank2_enable = 1;

		ram_copy(ram, &mem[0xE000], 0x2000);
		ram_copy(bank2, &mem[0xD000], 0x1000);
		writep(set_writep);
		break;

	case 0x01:			/* C081 */
		ram_read  = 0;
		ram_write = 1;
		bank2_enable = 1;

		ram_copy(rom, &mem[0xD000], 0x3000);
		writep(set_ramwrite);
		break;

	case 0x02:			/* C082 */
		ram_read  = 0;
		ram_write = 0;
		bank2_enable = 1;

		ram_copy(rom, &mem[0xD000], 0x3000);
		writep(set_writep);
		break;

	case 0x03:			/* C083 */
		ram_read  = 1;
		ram_write = 1;
		bank2_enable = 1;

		ram_copy(ram, &mem[0xE000], 0x2000);
		ram_copy(bank2, &mem[0xD000], 0x1000);
		writep(NULL);
		break;

	case 0x08: 			/* C088 */
		ram_read  = 1;
		ram_write = 0;
		bank2_enable = 0;

		ram_copy(ram, &mem[0xE000], 0x2000);
		ram_copy(bank1, &mem[0xD000], 0x1000);
		writep(set_writep);
		break;

	case 0x09:			/* C089 */
		ram_read  = 0;
		ram_write = 1;
		bank2_enable = 0;
		ram_copy(rom, &mem[0xD000], 0x3000);
		writep(set_ramwrite);
		break;

	case 0x0a:			/* C08A */
		ram_read  = 0;
		ram_write = 0;
		bank2_enable = 0;
		ram_copy(rom, &mem[0xD000], 0x3000);
		writep(set_writep);
		break;

	case 0x0b:			/* C08B */
		ram_read  = 1;
		ram_write = 1;
		bank2_enable = 0;
		ram_copy(ram, &mem[0xE000], 0x2000);
		ram_copy(bank1, &mem[0xD000], 0x1000);
		writep(NULL);
		break;
	}
}
