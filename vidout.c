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
#import "a2.h"


#define		WNDLFT		0x20
#define		WNDWDTH		0x21
#define		WNDTOP		0x22
#define		WNDBTM		0x23
#define		CH		0x24
#define		CV		0x25
#define		BASL		0x28
#define		BASH		0x29
#define		BAS2L		0x2A
#define		BAS2H		0x2B



/*
 *  SCROLL at $FC70
 */

void scroll(void)
{
int top;
unsigned short bas, bas2, ptr;
int width;
int i;
int scrl2_normal;

	if (mem[0x21] == 40 && mem[0x22] == 0 && mem[0x23] == 24) {
		MoveCursor(term_lines, 0);
		putchar('\n');
		last_line = -1;
		last_col = -1;
		fflush(stdout);
		scrl2_normal = FALSE;
	} else
		scrl2_normal = TRUE;

	top = mem[WNDTOP];
	width = mem[WNDWDTH] - 1;
	bas = text1[top % 32] + mem[WNDLFT];

	while (1) {
		bas2 = bas;

		if (++top >= mem[WNDBTM]) {
			mem[BASL] = low(bas);
			mem[BASH] = high(bas);
			Pc = 0xFC95;
			fflush(stdout);
			return;
		}

		bas = text1[top % 32] + mem[WNDLFT];
		ptr = bas;

		if (scrl2_normal)
			for (i = 0; i <= width; i++)
				set_text1f(bas2++, mem[ptr++]);
		else
			for (i = 0; i <= width; i++)
				mem[bas2++] = mem[ptr++];
	}
}

/*
 *  VIDOUT at $FBFD
 */

void vidout(void)
{
unsigned short ptr;

	if (A >= 0xA0 || A < 0x80) {
		ptr = join(mem[BASL], mem[BASH]) + mem[CH];
		set_text1f(ptr, A);
		mem[CH]++;
		if (mem[CH] >= mem[WNDWDTH])
			mem[CH] = 0;
		else {
			DO_RTS;
			return;
		}
	} else if (A == 0x8D)
		mem[CH] = 0;
	else if (A != 0x8A) {
		Pc = 0xFC0C;
		return;
	}

	A = ++mem[CV];
	if (A < mem[WNDBTM]) {
		Pc = 0xFC24;
		return;
	}
	mem[CV]--;

	scroll();
}
