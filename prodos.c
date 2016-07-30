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
#import	"a2.h"


#define		BSIZE	512		/* block size */
#define		D1SIZE	280		/* Size of disk 1 */
#define		D2SIZE	1024		/* Size of disk 2 */


/*
 *  Prodos to DOS 3.3 block mapping
 */

int conv1[] = { 0x00, 0x0D, 0x0B, 0x09, 0x07, 0x05, 0x03, 0x01 };
int conv2[] = { 0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x0F };



void proformat(void)
{

	if (write_prot[drive]) {
		C = 1;
		A = 0x2B;		/* Write protected */
		return;
	}

	C = 0;
	A = 0;
}


void proread(void)
{
int block;
unsigned short ptr;
int track, sector;
char s[50];

	block = join(mem[0x46], mem[0x47]);
	ptr = join(mem[0x44], mem[0x45]);

	sprintf(s, "proread b=%d", block);
	info(s);

	track = block / 8;
	sector = conv1[block % 8];

	read_disk(track, sector, &mem[ptr]);
	ptr += 0x100;

	sector = conv2[block % 8];
	read_disk(track, sector, &mem[ptr]);

	C = 0;
	A = 0;
}


void prowrite(void)
{
int block;
unsigned short ptr;
int track;
int sector;
char s[50];

	if (write_prot[drive]) {
		C = 1;
		A = 0x2B;		/* Write protected */
		return;
	}

	block = join(mem[0x46], mem[0x47]);
	ptr = join(mem[0x44], mem[0x45]);

	sprintf(s, "prowrite b=%d", block);
	info(s);

	track = block / 8;
	sector = conv1[block % 8];

	write_disk(track, sector, &mem[ptr]);
	ptr += 0x100;

	sector = conv2[block % 8];
	write_disk(track, sector, &mem[ptr]);

	C = 0;
	A = 0;
}


void prostatus(void)
{

	C = 0;
	A = 0;
	Y = high(D1SIZE);
	X = low(D1SIZE);
}


void prodos(void)
{

	if (mem[0x43] < 0x80)
		drive = 0;
	else
		drive = 1;

	if (disk[drive] < 0) {
		C = 1;
		A = 0x27;		/* IO Error */

	} else switch (mem[0x42]) {
		case 0: 
			prostatus();
			break;
		case 1:
			proread();
			break;
		case 2:
			prowrite();
			break;
		case 3:
			proformat();
			break;
	}

	DO_RTS;
}
