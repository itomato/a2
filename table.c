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



struct op_info opcode[] = {
	{"brk",	M_NONE},	/* 0x0 */
	{"ora",	M_INDX},	/* 0x1 */
	{"",	M_NONE},	/* 0x2 */
	{"",	M_NONE},	/* 0x3 */
	{"tsb",	M_ZP},		/* 0x4 */	/* 65C02 */
	{"ora",	M_ZP},		/* 0x5 */
	{"asl",	M_ZP},		/* 0x6 */
	{"",	M_NONE},	/* 0x7 */
	{"php",	M_NONE},	/* 0x8 */
	{"ora",	M_IMM},		/* 0x9 */
	{"asl",	M_NONE},	/* 0xa */
	{"",	M_NONE},	/* 0xb */
	{"tsb",	M_ABS},		/* 0xc */	/* 65C02 */
	{"ora",	M_ABS},		/* 0xd */
	{"asl",	M_ABS},		/* 0xe */
	{"",	M_NONE},	/* 0xf */
	{"bpl",	M_REL},		/* 0x10 */
	{"ora",	M_INDY},	/* 0x11 */
	{"ora",	M_ZIND},	/* 0x12 */	/* 65C02 */
	{"",	M_NONE},	/* 0x13 */
	{"trb",	M_ZP},		/* 0x14 */	/* 65C02 */
	{"ora",	M_ZPX},		/* 0x15 */
	{"asl",	M_ZPX},		/* 0x16 */
	{"",	M_NONE},	/* 0x17 */
	{"clc",	M_NONE},	/* 0x18 */
	{"ora",	M_ABSY},	/* 0x19 */
	{"ina",	M_NONE},	/* 0x1a */	/* 65C02 */
	{"",	M_NONE},	/* 0x1b */
	{"trb",	M_ABS},		/* 0x1c */	/* 65C02 */
	{"ora",	M_ABSX},	/* 0x1d */
	{"asl",	M_ABSX},	/* 0x1e */
	{"",	M_NONE},	/* 0x1f */
	{"jsr",	M_ABS},		/* 0x20 */
	{"and",	M_INDX},	/* 0x21 */
	{"",	M_NONE},	/* 0x22 */
	{"",	M_NONE},	/* 0x23 */
	{"bit",	M_ZP},		/* 0x24 */
	{"and",	M_ZP},		/* 0x25 */
	{"rol",	M_ZP},		/* 0x26 */
	{"",	M_NONE},	/* 0x27 */
	{"plp",	M_NONE},	/* 0x28 */
	{"and",	M_IMM},		/* 0x29 */
	{"rol",	M_NONE},	/* 0x2a */
	{"",	M_NONE},	/* 0x2b */
	{"bit",	M_ABS},		/* 0x2c */
	{"and",	M_ABS},		/* 0x2d */
	{"rol",	M_ABS},		/* 0x2e */
	{"",	M_NONE},	/* 0x2f */
	{"bmi",	M_REL},		/* 0x30 */
	{"and",	M_INDY},	/* 0x31 */
	{"and",	M_ZIND},	/* 0x32 */	/* 65C02 */
	{"",	M_NONE},	/* 0x33 */
	{"bit",	M_ZPX},		/* 0x34 */	/* 65C02 */
	{"and",	M_ZPX},		/* 0x35 */
	{"rol",	M_ZPX},		/* 0x36 */
	{"",	M_NONE},	/* 0x37 */
	{"sec",	M_NONE},	/* 0x38 */
	{"and",	M_ABSY},	/* 0x39 */
	{"dey",	M_NONE},	/* 0x3a */	/* 65C02 */
	{"",	M_NONE},	/* 0x3b */
	{"bit",	M_ABSX},	/* 0x3c */	/* 65C02 */
	{"and",	M_ABSX},	/* 0x3d */
	{"rol",	M_ABSX},	/* 0x3e */
	{"",	M_NONE},	/* 0x3f */
	{"rti",	M_NONE},	/* 0x40 */
	{"eor",	M_INDX},	/* 0x41 */
	{"",	M_NONE},	/* 0x42 */
	{"",	M_NONE},	/* 0x43 */
	{"",	M_NONE},	/* 0x44 */
	{"eor",	M_ZP},		/* 0x45 */
	{"lsr",	M_ZP},		/* 0x46 */
	{"",	M_NONE},	/* 0x47 */
	{"pha",	M_NONE},	/* 0x48 */
	{"eor",	M_IMM},		/* 0x49 */
	{"lsr",	M_NONE},	/* 0x4a */
	{"",	M_NONE},	/* 0x4b */
	{"jmp",	M_ABS},		/* 0x4c */
	{"eor",	M_ABS},		/* 0x4d */
	{"lsr",	M_ABS},		/* 0x4e */
	{"",	M_NONE},	/* 0x4f */
	{"bvc",	M_REL},		/* 0x50 */
	{"eor",	M_INDY},	/* 0x51 */
	{"eor",	M_ZIND},	/* 0x52 */	/* 65C02 */
	{"",	M_NONE},	/* 0x53 */
	{"",	M_NONE},	/* 0x54 */
	{"eor",	M_ZPX},		/* 0x55 */
	{"lsr",	M_ZPX},		/* 0x56 */
	{"",	M_NONE},	/* 0x57 */
	{"cli",	M_NONE},	/* 0x58 */
	{"eor",	M_ABSY},	/* 0x59 */
	{"phy",	M_NONE},	/* 0x5a */	/* 65C02 */
	{"",	M_NONE},	/* 0x5b */
	{"",	M_NONE},	/* 0x5c */
	{"eor",	M_ABSX},	/* 0x5d */
	{"lsr",	M_ABSX},	/* 0x5e */
	{"",	M_NONE},	/* 0x5f */
	{"rts",	M_NONE},	/* 0x60 */
	{"adc",	M_INDX},	/* 0x61 */
	{"",	M_NONE},	/* 0x62 */
	{"",	M_NONE},	/* 0x63 */
	{"stz",	M_ZP},		/* 0x64 */	/* 65C02 */
	{"adc",	M_ZP},		/* 0x65 */
	{"ror",	M_ZP},		/* 0x66 */
	{"",	M_NONE},	/* 0x67 */
	{"pla",	M_NONE},	/* 0x68 */
	{"adc",	M_IMM},		/* 0x69 */
	{"ror",	M_NONE},	/* 0x6a */
	{"",	M_NONE},	/* 0x6b */
	{"jmp",	M_IND},		/* 0x6c */
	{"adc",	M_ABS},		/* 0x6d */
	{"ror",	M_ABS},		/* 0x6e */
	{"",	M_NONE},	/* 0x6f */
	{"bvs",	M_REL},		/* 0x70 */
	{"adc",	M_INDX},	/* 0x71 */
	{"adc",	M_ZIND},	/* 0x72 */	/* 65C02 */
	{"",	M_NONE},	/* 0x73 */
	{"stz",	M_ZPX},		/* 0x74 */	/* 65C02 */
	{"adc",	M_ZPX},		/* 0x75 */
	{"ror",	M_ZPX},		/* 0x76 */
	{"",	M_NONE},	/* 0x77 */
	{"sei",	M_NONE},	/* 0x78 */
	{"adc",	M_ABSY},	/* 0x79 */
	{"ply",	M_NONE},	/* 0x7a */	/* 65C02 */
	{"",	M_NONE},	/* 0x7b */
	{"jmp",	M_ABINDX},	/* 0x7c */	/* 65C02 */
	{"adc",	M_ABSX},	/* 0x7d */
	{"ror",	M_ABSX},	/* 0x7e */
	{"",	M_NONE},	/* 0x7f */
	{"bra",	M_REL},		/* 0x80 */	/* 65C02 */
	{"sta",	M_INDX},	/* 0x81 */
	{"",	M_NONE},	/* 0x82 */
	{"",	M_NONE},	/* 0x83 */
	{"sty",	M_ZP},		/* 0x84 */
	{"sta",	M_ZP},		/* 0x85 */
	{"stx",	M_ZP},		/* 0x86 */
	{"",	M_NONE},	/* 0x87 */
	{"dey",	M_NONE},	/* 0x88 */
	{"",	M_NONE},	/* 0x89 */
	{"txa",	M_NONE},	/* 0x8a */
	{"",	M_NONE},	/* 0x8b */
	{"sty",	M_ABS},		/* 0x8c */
	{"sta",	M_ABS},		/* 0x8d */
	{"stx",	M_ABS},		/* 0x8e */
	{"",	M_NONE},	/* 0x8f */
	{"bcc",	M_REL},		/* 0x90 */
	{"sta",	M_INDY},	/* 0x91 */
	{"sta",	M_ZIND},	/* 0x92 */	/* 65C02 */
	{"",	M_NONE},	/* 0x93 */
	{"sty",	M_ZPX},		/* 0x94 */
	{"sta",	M_ZPX},		/* 0x95 */
	{"stx",	M_ZPX},		/* 0x96 */
	{"",	M_NONE},	/* 0x97 */
	{"tya",	M_NONE},	/* 0x98 */
	{"sta",	M_ABSY},	/* 0x99 */
	{"txs",	M_NONE},	/* 0x9a */
	{"",	M_NONE},	/* 0x9b */
	{"stz",	M_ABS},		/* 0x9c */	/* 65C02 */
	{"sta",	M_ABSX},	/* 0x9d */
	{"stz",	M_ABSX},	/* 0x9e */	/* 65C02 */
	{"",	M_NONE},	/* 0x9f */
	{"ldy",	M_IMM},		/* 0xa0 */
	{"lda",	M_INDX},	/* 0xa1 */
	{"ldx",	M_IMM},		/* 0xa2 */
	{"",	M_NONE},	/* 0xa3 */
	{"ldy",	M_ZP},		/* 0xa4 */
	{"lda",	M_ZP},		/* 0xa5 */
	{"ldx",	M_ZP},		/* 0xa6 */
	{"",	M_NONE},	/* 0xa7 */
	{"tay",	M_NONE},	/* 0xa8 */
	{"lda",	M_IMM},		/* 0xa9 */
	{"tax",	M_NONE},	/* 0xaa */
	{"",	M_NONE},	/* 0xab */
	{"ldy",	M_ABS},		/* 0xac */
	{"lda",	M_ABS},		/* 0xad */
	{"ldx",	M_ABS},		/* 0xae */
	{"",	M_NONE},	/* 0xaf */
	{"bcs",	M_REL},		/* 0xb0 */
	{"lda",	M_INDY},	/* 0xb1 */
	{"lda",	M_ZIND},	/* 0xb2 */	/* 65C02 */
	{"",	M_NONE},	/* 0xb3 */
	{"ldy",	M_ZPX},		/* 0xb4 */
	{"lda",	M_ZPX},		/* 0xb5 */
	{"ldx",	M_ZPY},		/* 0xb6 */
	{"",	M_NONE},	/* 0xb7 */
	{"clv",	M_NONE},	/* 0xb8 */
	{"lda",	M_ABSY},	/* 0xb9 */
	{"tsx",	M_NONE},	/* 0xba */
	{"",	M_NONE},	/* 0xbb */
	{"ldy",	M_ABSX},	/* 0xbc */
	{"lda",	M_ABSX},	/* 0xbd */
	{"ldx",	M_ABSY},	/* 0xbe */
	{"",	M_NONE},	/* 0xbf */
	{"cpy",	M_IMM},		/* 0xc0 */
	{"cmp",	M_INDX},	/* 0xc1 */
	{"",	M_NONE},	/* 0xc2 */
	{"",	M_NONE},	/* 0xc3 */
	{"cpy",	M_ZP},		/* 0xc4 */
	{"cmp",	M_ZP},		/* 0xc5 */
	{"dec",	M_ZP},		/* 0xc6 */
	{"",	M_NONE},	/* 0xc7 */
	{"iny",	M_NONE},	/* 0xc8 */
	{"cmp",	M_IMM},		/* 0xc9 */
	{"dex",	M_NONE},	/* 0xca */
	{"",	M_NONE},	/* 0xcb */
	{"cpy",	M_ABS},		/* 0xcc */
	{"cmp",	M_ABS},		/* 0xcd */
	{"dec",	M_ABS},		/* 0xce */
	{"",	M_NONE},	/* 0xcf */
	{"bne",	M_REL},		/* 0xd0 */
	{"cmp",	M_INDY},	/* 0xd1 */
	{"cmp",	M_ZIND},	/* 0xd2 */	/* 65C02 */
	{"",	M_NONE},	/* 0xd3 */
	{"",	M_NONE},	/* 0xd4 */
	{"cmp",	M_ZPX},		/* 0xd5 */
	{"dec",	M_ZPX},		/* 0xd6 */
	{"",	M_NONE},	/* 0xd7 */
	{"cld",	M_NONE},	/* 0xd8 */
	{"cmp",	M_ABSY},	/* 0xd9 */
	{"phx",	M_NONE},	/* 0xda */	/* 65C02 */
	{"",	M_NONE},	/* 0xdb */
	{"",	M_NONE},	/* 0xdc */
	{"cmp",	M_ABSX},	/* 0xdd */
	{"dec",	M_ABSX},	/* 0xde */
	{"",	M_NONE},	/* 0xdf */
	{"cpx",	M_IMM},		/* 0xe0 */
	{"sbc",	M_INDX},	/* 0xe1 */
	{"",	M_NONE},	/* 0xe2 */
	{"",	M_NONE},	/* 0xe3 */
	{"cpx",	M_ZP},		/* 0xe4 */
	{"sbc",	M_ZP},		/* 0xe5 */
	{"inc",	M_ZP},		/* 0xe6 */
	{"",	M_NONE},	/* 0xe7 */
	{"inx",	M_NONE},	/* 0xe8 */
	{"sbc",	M_IMM},		/* 0xe9 */
	{"nop",	M_NONE},	/* 0xea */
	{"",	M_NONE},	/* 0xeb */
	{"cpx",	M_ABS},		/* 0xec */
	{"sbc",	M_ABS},		/* 0xed */
	{"inc",	M_ABS},		/* 0xee */
	{"",	M_NONE},	/* 0xef */
	{"beq",	M_REL},		/* 0xf0 */
	{"sbc",	M_INDY},	/* 0xf1 */
	{"sbc",	M_ZIND},	/* 0xf2 */	/* 65C02 */
	{"",	M_NONE},	/* 0xf3 */
	{"",	M_NONE},	/* 0xf4 */
	{"sbc",	M_ZPX},		/* 0xf5 */
	{"inc",	M_ZPX},		/* 0xf6 */
	{"",	M_NONE},	/* 0xf7 */
	{"sed",	M_NONE},	/* 0xf8 */
	{"sbc",	M_ABSY},	/* 0xf9 */
	{"plx",	M_NONE},	/* 0xfa */	/* 65C02 */
	{"",	M_NONE},	/* 0xfb */
	{"",	M_NONE},	/* 0xfc */
	{"sbc",	M_ABSX},	/* 0xfd */
	{"inc",	M_ABSX},	/* 0xfe */
	{"",	M_NONE},	/* 0xff */
};


int diss(unsigned short pc, FILE *fp)
{
char *s;
int count;
int tmp;
unsigned short addr;
unsigned char one, two, three;

	fprintf(fp, "%.4X:  ", pc);

	addr = pc;
	one = mem[pc++];
	two = mem[pc++];
	three = mem[pc++];

	s = opcode[one].name;

	if (s == NULL || *s == '\0')
		s = "???";

	switch (opcode[one].add_mode) {
		case 0:
			fprintf(fp, "%.2x        %s           ", one, s);
			count = 1;
			break;

		case M_ZP:
			fprintf(fp, "%.2x %.2x     %s  $%.2x      ",
							one, two, s, two);
			count = 2;
			break;

		case M_ZPX:
			fprintf(fp, "%.2x %.2x     %s  $%.2x,X    ",
							one, two, s, two);
			count = 2;
			break;

		case M_ZPY:
			fprintf(fp, "%.2x %.2x     %s  $%.2x,Y    ",
							one, two, s, two);
			count = 2;
			break;

		case M_IND:
			fprintf(fp, "%.2x %.2x %.2x  %s  ($%.4x)  ",
						one, two, three, s,
						join(two, three));
			count = 3;
			break;

		case M_INDX:
			fprintf(fp, "%.2x %.2x     %s  ($%.2x,X)  ",
						one, two, s, two);
			count = 2;
			break;

		case M_INDY:
			fprintf(fp, "%.2x %.2x     %s  ($%.2x),Y  ",
						one, two, s, two);
			count = 2;
			break;

		case M_ABS:
			fprintf(fp, "%.2x %.2x %.2x  %s  $%.4x    ",
						one, two, three, s,
						join(two, three));
			count = 3;
			break;

		case M_ABSX:
			fprintf(fp, "%.2x %.2x %.2x  %s  $%.4x,X  ",
						one, two, three, s,
						join(two, three));
			count = 3;
			break;

		case M_ABSY:
			fprintf(fp, "%.2x %.2x %.2x  %s  $%.4x,Y  ",
						one, two, three, s,
						join(two, three));
			count = 3;
			break;

		case M_IMM:
			fprintf(fp, "%.2x %.2x     %s  #$%.2x     ",
						one, two, s, two);
			count = 2;
			break;

		case M_REL:
			tmp = two;
			if (tmp > 127)
				tmp = tmp - 256;
			tmp += addr + 2;
			tmp &= 0xFFFF;

			fprintf(fp, "%.2x %.2x     %s  $%.4x    ",
						one, two, s, tmp);
			count = 2;
			break;

		case M_ZIND:
			fprintf(fp, "%.2x %.2x     %s  ($%.2x)    ",
						one, two, s, two);
			count = 2;
			break;

		case M_ABINDX:
			fprintf(fp, "%.2x %.2x %.2x  %s  ($%.4x,X)",
						one, two, three, s,
						join(two, three));
			count = 3;
			break;

		default:
			fprintf(fp, "???        %c             ", one);
			count = 1;
	}

	return(count);
}

void flags(FILE *fp)
{
	fprintf(fp, " A=%.2X X=%.2X Y=%.2X SP=%.2X", A, X, Y, Sp);
#ifdef	JOYSTICK_SUPPORT
	fprintf(fp, "  N%d V%d B%d D%d I%d Z%d C%d JX=%.4X JY=%.4X\n",
		!!N, !!V, !!B, !!D, !!I, !NZ, !!C,
		joystick_counter_x, joystick_counter_y);
#else	JOYSTICK_SUPPORT
	fprintf(fp, "  N%d V%d B%d D%d I%d Z%d C%d\n",
				!!N, !!V, !!B, !!D, !!I, !NZ, !!C);
#endif	JOYSTICK_SUPPORT
}
