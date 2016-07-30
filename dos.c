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


#include	<stdio.h>
#include	"a2.h"



char *rwts_func[] = {"-s", "", "-w", "-?", "-f"};


void rwts(void)
{
char s[50];
unsigned short iob;
unsigned short buf;
int track;
int sector;
int cmd;
unsigned char zero_buf[256];
int i;

	mem[0x48] = Y;
	mem[0x49] = A;
	mem[0x6F8] = 2;
	mem[0x4F8] = 4;

	iob = join(Y, A);

	track = mem[iob + 4];
	sector = mem[iob + 5];
	buf = join(mem[iob + 8], mem[iob + 9]);
	cmd = mem[iob + 12];

	if (cmd > 4)
		cmd = 3;		/* unknown command */

	if (mem[iob + 2] == 2)
		drive = 1;
	else
		drive = 0;

	if ((cmd == 2 || cmd == 4) && write_prot[drive]) {
		C = 1;
		mem[iob + 13] = 0x10;		/* Write protected */
		DO_RTS;
		return;
	}

	if (mem[iob] != 1
	||  disk[drive] < 0
	||  track > 35
	||  sector > 15
	||  cmd == 3
	||  buf > 0xFE00) {
		C = 1;
		mem[iob + 13] = 0x40;		/* IO Error */
		DO_RTS;
		return;
	}

	sprintf(s, "rwts%s t=%d s=%d%s", rwts_func[cmd],
				track, sector, drive ? " d2" : "");
	info(s);

	switch (cmd) {
	case 0:					/* seek */
		break;

	case 1:					/* read */
		read_disk(track, sector, &mem[buf]);
		break;

	case 2:					/* write */
		write_disk(track, sector, &mem[buf]);
		break;

	case 4:					/* format */
		for (i = 0; i < 256; i++)
			zero_buf[i] = 0;

		for (track = 0; track < 35; track++)
			for (sector = 0; sector < 16; sector++)
				write_disk(track, sector, zero_buf);
		break;
	}

	C = 0;
	A = 0;
	DO_RTS;
}

