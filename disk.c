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


#define		GAP	0x7F;		/* data gap byte */


/*
 *  4 by 4 nibble encoding macros
 */

#define		nib1(a)	(((a) >> 1) | 0xAA)
#define		nib2(a)	((a) | 0xAA)
#define		denib(a,b)	(((((a) & 0x55) << 1) & 0xFF) | ((b) & 0x55))


int cur_track[2] = {0, 0};
int sect_pos[2];		/* current sector within track */

extern unsigned char tab1[];	/* For nibblizing.  At end of this file */
extern unsigned char tab2[];	/* Disk byte translation table. */
extern unsigned char phys[];	/* DOS 3.3 to physical sector mapping */

unsigned char sect_buf[2][1024];
unsigned char *sectp = NULL;
unsigned char *sect_point[2] = {NULL, NULL};
unsigned char write_reg;	/* write data register */
int write_mode = FALSE;



/*
 *  Determine what track the disk head is over by watching toggles to
 *  the four stepper motor magnets.
 */

void step_motor(unsigned short a)
{
static	int mag[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}},
	pmag[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}},	/* previous */
	ppmag[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}},	/* previous previous */
	pnum[2] = {0, 0},
	ppnum[2] = {0, 0},
	track_pos[2] = {0, 0};

static int prev_track[2] = {0, 0};
int magnet_number;

	a &= 7;
	magnet_number = a >> 1;

	ppmag[drive][ppnum[drive]] = pmag[drive][ppnum[drive]];
	ppnum[drive] = pnum[drive];

	pmag[drive][pnum[drive]] = mag[drive][pnum[drive]];
	pnum[drive] = magnet_number;

	if ((a & 1) == 0)
		mag[drive][magnet_number] = FALSE;
	else {
		if (ppmag[drive][(magnet_number + 1) & 3]) {
			track_pos[drive]--;
			if (track_pos[drive] < 0) {
				track_pos[drive] = 0;
				if (drive)
					info("recal d2");
				else
					info("recal");
			}
		}

		if (ppmag[drive][(magnet_number - 1) & 3]) {
			track_pos[drive]++;
			if (track_pos[drive] > 140)
				track_pos[drive] = 140;

		}
		mag[drive][magnet_number] = TRUE;
	}

	cur_track[drive] = (track_pos[drive] + 1) / 2;
	if (cur_track[drive] != prev_track[drive]) {
#if 0
		sprintf(s, "step to %d%s", cur_track[drive],
						drive ? " d2" : "");
		info(s);
#endif
		sectp[drive] = 0;  /* recompute sector if head moves */
		sect_pos[drive] = 0;
		prev_track[drive] = cur_track[drive];
	}
}


/*
 *  Take a normal 256 byte sector and nibblize it into 342 bytes
 *  Checksum it with itself; this yields one more byte.
 *  Translate the resulting 343 bytes into "disk bytes".
 *  Insert them into the sector buffer.
 *
 *  See a reference such as _Beneath Apple Prodos_ for an explanation
 *  of why & how this is done.
 */

void read_disk(int track, int sector, unsigned char *buf)
{
  long block;

  block = track * 16 + sector;

  lseek(disk[drive], block * 256, 0);
  if (read(disk[drive], buf, 256) != 256)
    perror("bad read");
}


void write_disk(int track, int sector, unsigned char *buf)
{
  long block;

  block = track * 16 + sector;

  lseek(disk[drive], block * 256, 0);
  if (write(disk[drive], buf, 256) != 256)
    perror("bad write");
}

void encode_data(int track, int sector)
{
unsigned char buf[344];
unsigned char *one;
unsigned char *bump;
unsigned char *two;
unsigned char *three;
unsigned char *dest;
int i;

	read_disk(track, sector, &buf[86]);
	buf[342] = 0;
	buf[343] = 0;

	dest = buf;
	one = &buf[86];
	two = &buf[86 + 0x56];
	bump = two;
	three = &buf[86 + 0xAC];

	do {
		i = (*one++ & 0x03) |
			((*two++ & 0x03) << 2) | ((*three++ & 0x03) << 4);

		*dest++ = tab1[i];
	} while (one != bump);

	sectp[0] = buf[0];
	for (i = 1; i <= 342; i++)
		sectp[i] = buf[i - 1] ^ buf[i];

	for (i = 0; i <= 342; i++)
		sectp[i] = tab2[ sectp[i] >> 2 ];

	sectp = &sectp[343];
}

void setup_sector(int track, int sector)
{
int checksum;
int physical_sector;
char s[50];
int i;

	physical_sector = phys[sector];

	sprintf(s, "raw t=%d s=%d%s", track, sector, drive ? " d2" : "");
	info(s);

	sectp = sect_buf[drive];

	for (i = 0; i < 16; i++)
		*sectp++ = GAP;

	*sectp++ = 0xD5;			/* address header */
	*sectp++ = 0xAA;
	*sectp++ = 0x96;

	*sectp++ = 0xFF;			/* disk volume 254 */
	*sectp++ = 0xFE;

	*sectp++ = nib1(track);
	*sectp++ = nib2(track);

	*sectp++ = nib1(physical_sector);
	*sectp++ = nib2(physical_sector);

	checksum = 254 ^ track ^ physical_sector;
	*sectp++ = nib1(checksum);
	*sectp++ = nib2(checksum);

	*sectp++ = 0xDE;			/* address trailer */
	*sectp++ = 0xAA;

	for (i = 0; i < 8; i++)
		*sectp++ = GAP;

	*sectp++ = 0xD5;			/* data header */
	*sectp++ = 0xAA;
	*sectp++ = 0xAD;

	encode_data(track, sector);		/* nibblized data */

	*sectp++ = 0xDE;			/* data trailer */
	*sectp++ = 0xAA;
	*sectp++ = 0xEB;

	*sectp = '\0';				/* ending mark for our use */
	sectp = sect_buf[drive];		/* start reading at beginning */
}


void raw_disk_write(void)
{
  printf("raw write %.2X\n", write_reg);
}


/*
 *  Helps with the bit fiddling necessary to extract the bottom
 *  two bits during the 256 - 342 byte nibblize.
 */

unsigned char tab1[] = {
	0x00, 0x08, 0x04, 0x0C, 0x20, 0x28, 0x24, 0x2C,
	0x10, 0x18, 0x14, 0x1C, 0x30, 0x38, 0x34, 0x3C,
	0x80, 0x88, 0x84, 0x8C, 0xA0, 0xA8, 0xA4, 0xAC,
	0x90, 0x98, 0x94, 0x9C, 0xB0, 0xB8, 0xB4, 0xBC,
	0x40, 0x48, 0x44, 0x4C, 0x60, 0x68, 0x64, 0x6C,
	0x50, 0x58, 0x54, 0x5C, 0x70, 0x78, 0x74, 0x7C,
	0xC0, 0xC8, 0xC4, 0xCC, 0xE0, 0xE8, 0xE4, 0xEC,
	0xD0, 0xD8, 0xD4, 0xDC, 0xF0, 0xF8, 0xF4, 0xFC,
};


/*
 *  Translates to "disk bytes"
 */

unsigned char tab2[] = {
	0x96, 0x97, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA6, 
	0xA7, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB3, 
	0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC, 
	0xBD, 0xBE, 0xBF, 0xCB, 0xCD, 0xCE, 0xCF, 0xD3, 
	0xD6, 0xD7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 
	0xDF, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xEC, 
	0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 
};


/*
 *  Dos 3.3 to physical sector conversion
 */

unsigned char phys[] = {
	0x00, 0x0D, 0x0B, 0x09, 0x07, 0x05, 0x03, 0x01,
	0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x0F,
};

unsigned char disk_ref(unsigned short a, unsigned char n)
{
	switch (a) {
	case 0xC0E0:	/* Phase 0 off */
	case 0xC0E1:	/* Phase 0 on */
	case 0xC0E2:	/* Phase 1 off */
	case 0xC0E3:	/* Phase 1 on */
	case 0xC0E4:	/* Phase 2 off */
	case 0xC0E5:	/* Phase 2 on */
	case 0xC0E6:	/* Phase 3 off */
	case 0xC0E7:	/* Phase 3 on */
		step_motor(a);
		break;

	case 0xC0E8:	/* Drive off */
		break;

	case 0xC0E9:	/* Drive on */
		break;

	case 0xC0EA:	/* Select drive 1 */
		sect_point[drive] = sectp;
		drive = 0;
		sectp = sect_point[0];
		break;

	case 0xC0EB:	/* Select drive 2 */
		sect_point[drive] = sectp;
		drive = 1;
		sectp = sect_point[1];
		break;

	case 0xC0EC:	/* Shift data register */
		if (disk[drive] < 0)
			return(0xFF);

		if (write_mode) {
			raw_disk_write();
			return(0);
		}

		if (sectp == NULL || *sectp == '\0') {
			sect_pos[drive]--;
			if (sect_pos[drive] < 0)
				sect_pos[drive] = 15;
			setup_sector(cur_track[drive], sect_pos[drive]);
		}

		return(*sectp++);

	case 0xC0ED:	/* Load data register */
		write_reg = n;
		break;

	case 0xC0EE:	/* Read mode */
		write_mode = FALSE;
		return(write_prot[drive] ? 0xFF : 0);
		break;

	case 0xC0EF:	/* Write mode */
		write_mode = TRUE;
		break;
	}

	return(0);
}
