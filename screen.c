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
#import <libc.h>
#import	"a2.h"

extern char screen_map[];		/* at the bottom of this file */
extern int text1[];


#ifdef	TEXT2
int line1[0x400];
int col1[0x400];
int line2[0x400];
int col2[0x400];
#else	TEXT2
int line1[0x400];
int col1[0x400];
#endif	TEXT2


void info(char *s)
{

	MoveCursor(0, 57);
	printf("%-20s", s);

	last_line = -1;
	last_col = -1;
	fflush(stdout);
}

#define appleout(n)	if (n >= 0x80) putchar(screen_map[n]); else { StartInverse(); putchar(screen_map[n]); EndInverse(); }


void set_text1f(unsigned short addr, unsigned char n)
{
int line, col;
unsigned char c;

	if (mem[addr] == n)		/* no change, don't do anything */
		return;

	mem[addr] = n;
	line = line1[addr - 0x400];
	col = col1[addr - 0x400];

	if (line == -1)
		return;

/*
 *  Stuff to try to make updating more efficient
 *  Is it really worth it?
 */

	if (line == last_line) {
		if (col == 0)
			putchar('\r');
		else switch (col - last_col) {
		case 4:
			c = mem[addr - 3]; appleout(c);
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 3:
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 2:
			c = mem[addr - 1]; appleout(c);
			break;
		case 1:			/* already there */
			break;
		case 0:
			putchar('\b');
			break;
		case -1:
			putchar('\b'); putchar('\b');
			break;
		case -2:
			putchar('\b'); putchar('\b'); putchar('\b');
			break;
		case -3:
			putchar('\b'); putchar('\b'); putchar('\b');
			putchar('\b');
			break;
		default:
			MoveCursor(line, col);
		}
	} else
		MoveCursor(line, col);

	appleout(n);
	fflush(stdout);

	last_line = line;
	last_col = col;
}

#ifdef	TEXT2
void set_text2f(unsigned short addr, unsigned char n)
{
int line, col;
unsigned char c;

	if (mem[addr] == n)		/* no change, don't do anything */
		return;

	mem[addr] = n;
	line = line2[addr - 0x800];
	col = col2[addr - 0x800];

	if (line == -1)
		return;

/*
 *  Stuff to try to make updating more efficient
 *  Is it really worth it?
 */

	if (line == last_line) {
		if (col == 0)
			putchar('\r');
		else switch (col - last_col) {
		case 4:
			c = mem[addr - 3]; appleout(c);
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 3:
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 2:
			c = mem[addr - 1]; appleout(c);
			break;
		case 1:			/* already there */
			break;
		case 0:
			putchar('\b');
			break;
		case -1:
			putchar('\b'); putchar('\b');
			break;
		case -2:
			putchar('\b'); putchar('\b'); putchar('\b');
			break;
		case -3:
			putchar('\b'); putchar('\b'); putchar('\b');
			putchar('\b');
			break;
		default:
			MoveCursor(line + 24, col);
		}
	} else
		MoveCursor(line + 24, col);

	appleout(n);
	fflush(stdout);

	last_line = line;
	last_col = col;
}
#endif	TEXT2

void screen_setup(void)
{
  int line, col;
  int addr, base;
  int i;

  for (i = 0; i < 0x400; i++) {
    line1[i] = -1;
    col1[i] = -1;
#ifdef	TEXT2
    line2[i] = -1;
    col2[i] = -1;
#endif	TEXT2
  }

  mem_set[0x04] = set_text1f;
  mem_set[0x05] = set_text1f;
  mem_set[0x06] = set_text1f;
  mem_set[0x07] = set_text1f;

#ifdef	TEXT2
  mem_set[0x08] = set_text2f;
  mem_set[0x09] = set_text2f;
  mem_set[0x0A] = set_text2f;
  mem_set[0x0B] = set_text2f;
#endif	TEXT2

  for (line = 0; line < 24; line++) {
    base = text1[line];
    for (col = 0; col < 40; col++) {
      addr = base + col;
      mem[addr] = 0xA0;
      line1[addr - 0x400] = line;
      col1[addr - 0x400] = col;
#ifdef	TEXT2
      mem[addr + 0x400] = 0xA0;
      line2[addr - 0x400] = line;
      col2[addr - 0x400] = col;
#endif	TEXT2
    }
  }
}


int last_line = -1;
int last_col = -1;



void set_text1(unsigned short addr, unsigned char n)
{
int line, col;
unsigned char c;

	if (mem[addr] == n)		/* no change, don't do anything */
		return;

	mem[addr] = n;
	line = line1[addr - 0x400];
	col = col1[addr - 0x400];

	if (line == -1)
		return;

/*
 *  Stuff to try to make updating more efficient
 *  Is it really worth it?
 */

	if (line == last_line) {
		if (col == 0)
			putchar('\r');
		else switch (col - last_col) {
		case 4:
			c = mem[addr - 3]; appleout(c);
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 3:
			c = mem[addr - 2]; appleout(c);
			c = mem[addr - 1]; appleout(c);
			break;
		case 2:
			c = mem[addr - 1]; appleout(c);
			break;
		case 1:			/* already there */
			break;
		case 0:
			putchar('\b');
			break;
		case -1:
			putchar('\b'); putchar('\b');
			break;
		case -2:
			putchar('\b'); putchar('\b'); putchar('\b');
			break;
		case -3:
			putchar('\b'); putchar('\b'); putchar('\b');
			putchar('\b');
			break;
		default:
			MoveCursor(line, col);
		}
	} else
		MoveCursor(line, col);

	appleout(n);

	last_line = line;
	last_col = col;
}


void redraw_screen(void)
{
int i, j;
unsigned short base;
unsigned char c;

	ClearScreen();
	for (i = 0; i < 24; i++) {
		base = text1[i];
		for (j = 0; j < 40; j++) {
			c = mem[base + j];
			appleout(c);
		}
		if (i < 23)
			putchar('\n');
	}

#ifdef	TEXT2
	for (i = 0; i < 24; i++) {
		base = text1[i] + 0x400;
		for (j = 0; j < 40; j++) {
			c = mem[base + j];
			appleout(c);
		}
		if (i < 23)
			putchar('\n');
	}
#endif	TEXT2

	last_line = -1;
	last_col = -1;

	fflush(stdout);
}


/*
 *  Screen display mapping table
 */

char screen_map[] = {

	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $00	*/
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $08	*/
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $10	*/
	'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $18	*/
	' ', '!', '"', '#', '$', '%', '&', '\'',	/* $20	*/
	'(', ')', '*', '+', ',', '-', '.', '/',		/* $28	*/
	'0', '1', '2', '3', '4', '5', '6', '7',		/* $30	*/
	'8', '9', ':', ';', '<', '=', '>', '?',		/* $38	*/

	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $40	*/
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $48	*/
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $50	*/
	'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $58	*/
	' ', '!', '"', '#', '$', '%', '&', '\'',	/* $60	*/
	'(', ')', '*', '+', ',', '-', '.', '/',		/* $68	*/
	'0', '1', '2', '3', '4', '5', '6', '7',		/* $70	*/
	'8', '9', ':', ';', '<', '=', '>', '?',		/* $78	*/

	'@', 'a', 'b', 'c', 'd', 'e', 'f', 'g',		/* $80	*/
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',		/* $88	*/
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',		/* $90	*/
	'x', 'y', 'z', '[', '\\',']', '^', '_',		/* $98	*/
	' ', '!', '"', '#', '$', '%', '&', '\'',	/* $A0	*/
	'(', ')', '*', '+', ',', '-', '.', '/',		/* $A8	*/
	'0', '1', '2', '3', '4', '5', '6', '7',		/* $B0	*/
	'8', '9', ':', ';', '<', '=', '>', '?',		/* $B8	*/

	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $C0	*/
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $C8	*/
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $D0	*/
	'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $D8	*/
	' ', '!', '"', '#', '$', '%', '&', '\'',	/* $E0	*/
	'(', ')', '*', '+', ',', '-', '.', '/',		/* $E8	*/
	'0', '1', '2', '3', '4', '5', '6', '7',		/* $F0	*/
	'8', '9', ':', ';', '<', '=', '>', '?',		/* $F8	*/

};

int text1[] = {
	0x400, 0x480, 0x500, 0x580, 0x600, 0x680, 0x700, 0x780,
	0x428, 0x4A8, 0x528, 0x5A8, 0x628, 0x6A8, 0x728, 0x7A8,
	0x450, 0x4D0, 0x550, 0x5D0, 0x650, 0x6D0, 0x750, 0x7D0,
	0x478, 0x4F8, 0x578, 0x5F8, 0x678, 0x6F8, 0x778, 0x7F8
};

