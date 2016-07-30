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


#ifndef	TRUE
#define		TRUE		1
#endif	TRUE
#ifndef	FALSE
#define		FALSE		0
#endif	FALSE

extern FILE	*sfp;

/*
 *  Assertion verifier
 */
#define	assert(p)	if(! (p)) asfail(__FILE__, __LINE__, "p");


#define		join(low, high)		((low) | ((high) << 8))
#define		low(x)			((x) & 0xFF)
#define		high(x)			((x) >> 8)

#define		tobinary(v)		((v >> 4) * 10 + (v & 0xF))
#define		tobcd(v)		((v % 10) | ((v / 10) << 4))

#define		push(s)			mem[0x100 | Sp--] = s;
#define		pop()			mem[0x100 | ++Sp]

#define		SIGN(x)			N = ((x) & 0x80)
#define		ZERO(x)			NZ = (x)
#define		CARRY(x)		C = ((x) > 0xFF)
#define		dCARRY(x)		C = ((x) > 99)


#define	REF(a)		((a & 0xF000) == 0xC000 ? mem_map(a) : mem[a])
#define	REFptr		((ptr & 0xF000) == 0xC000 ? mem_map(ptr) : mem[ptr])
#define	REFzero		mem[ptr]
#define	REFimm		mem[Pc++]

#define	SET(a,v)	if (mem_set[a >> 8]) (*mem_set[a >> 8])(a,v); else mem[a] = v;
#define	SETptr(v)	if (mem_set[ptr >> 8]) (*mem_set[ptr >> 8])(ptr,v); else mem[ptr] = v;
#define	SETzero(v)	mem[ptr] = v;

#define	PCINC		Pc++;

#define	ABSOL		ptr = mem[Pc++]; ptr |= mem[Pc++] << 8;
#define	ABSX		ptr = mem[Pc++]; ptr |= mem[Pc++] << 8; ptr += X;
#define	ABSY		ptr = mem[Pc++]; ptr |= mem[Pc++] << 8; ptr += Y;
#define	ZEROP		ptr = mem[Pc++];
#define	ZEROX		ptr = (mem[Pc++] + X) & 0xFF;
#define	ZEROY		ptr = (mem[Pc++] + Y) & 0xFF;
#define	INDX	stmp = mem[Pc++] + X; ptr = mem[stmp++]; ptr |= mem[stmp] << 8;
#define	INDY	stmp = mem[Pc++]; ptr = mem[stmp++]; ptr |= mem[stmp] << 8; ptr += Y;

#define	rZEROP		mem[ mem[Pc++] ]
#define	rZEROX		mem[ (mem[Pc++] + X) & 0xFF ]
#define	rZEROY		mem[ (mem[Pc++] + Y) & 0xFF ]
#define	sZEROP(v)	mem[ mem[Pc++] ] = v;
#define	sZEROX(v)	mem[ (mem[Pc++] + X) & 0xFF ] = v;
#define	sZEROY(v)	mem[ (mem[Pc++] + Y) & 0xFF ] = v;

#define	ZIND		ptr = mem[ mem[Pc++] ];
#define	ABINDX		stmp = mem[Pc++]; stmp |= mem[Pc++] << 8; ptr = mem[stmp++]; ptr |= mem[stmp] << 8; ptr += X;


/*
 *  Since we store flags as 0/nonzero instead of 0/1, we need to turn
 *  nonzero back into 1.  Thus, "!!" is not a no-op.
 */

#define	get_status()   ((!!N << 7)|(!!V << 6)|(!!B << 4)|(!!D << 3)|(!!I << 2)|(!NZ << 1)|(!!C))

extern unsigned char mem[];		   /* 64K memory image */
extern unsigned char jmp_tbl[];		   /* jmp & jsr interceptor functions */
extern void (*mem_set[256])(unsigned short, unsigned char);/* memory store interceptors */

extern unsigned char mem_map();


extern unsigned short Pc;
extern unsigned char Sp;	/* stack pointer 	*/
extern unsigned int A;		/* accumulator		*/
extern unsigned char X;		/* X register		*/
extern unsigned char Y;		/* Y register		*/

extern unsigned int N;		/* 7 - sign		*/
extern unsigned int V;		/* 6 - Overflow		*/
				/* 5 - Unused		*/
extern unsigned int B;		/* 4 - Break		*/
extern unsigned int D;		/* 3 - Decimal		*/
extern unsigned int I;		/* 2 - Interrupt	*/
extern unsigned int NZ;		/* 1 - inverse of Zero  */
extern unsigned int C;		/* 0 - Carry		*/


/*
 *  Note:
 *	Flags are stored as zero/nonzero instead of 0/1, except for
 *	N which is stored 0/0x80.  NZ is "not Z"; use !NZ to get
 *	the zero flag.
 *
 *	The flags code is tight, especially around overflow set on ADC
 *	and SBC.  Integer basic is a good test to see if it still works.
 */


#define		M_NONE		0
#define		M_ZP		1
#define		M_ZPX		2
#define		M_ZPY		3
#define		M_IND		4
#define		M_INDX		5
#define		M_INDY		6
#define		M_ABS		7
#define		M_ABSX		8
#define		M_ABSY		9
#define		M_IMM		10
#define		M_REL		11
#define		M_ZIND		12	/* zero page indirect, 65C02 */
#define		M_ABINDX	13	/* for jmp $7C in 65C02 */

#define	PRIMARY_PAGE	0
#define	SECONDARY_PAGE	1

extern int screen_page;

extern int ram_read;
extern int ram_write;
extern int bank2_enable;

struct op_info {
	char *name;
	int add_mode;
};

extern struct op_info opcode[];
extern int running;
extern int tracing;
extern char *my_malloc();
extern int term_lines, term_cols;

extern int text1[];		/* base addresses for text page 1 */
extern int last_line;		/* line the cursor is on now */
extern int last_col;		/* current column of cursor */
#ifdef	TEXT2
//extern int last_line2;		/* line the cursor is on now */
//extern int last_col2;		/* current column of cursor */
#endif	TEXT2

extern int disk[2];
extern int write_prot[2];
extern int drive;

#ifdef	JOYSTICK_SUPPORT
/* from main.c */
extern int joystick_fd;
extern int xmin, ymin;
extern int xcenter, ycenter;
extern float xpartition_low, ypartition_low;
extern float xpartition_high, ypartition_high;
extern int joystick_counter_x, joystick_counter_y;
#endif	JOYSTICK_SUPPORT

#define	DO_RTS	Pc = pop(); Pc |= pop() << 8; Pc++;

/* From curses.c */
void info(char *s);
void MoveCursor(int row, int col);
int InitScreen(void);
void ClearScreen(void);
void StartInverse(void);
void EndInverse(void);
void Raw(int state);
void ScreenSize(int *lines, int *columns);

/* From mem.c */
void memory_setup(void);
void ram_card(unsigned short addr);
void set_writep(unsigned short a, unsigned char n);

/* From jump.c */
void jump(int key);
void set_special_jumps(void);

/* From commands.c */
int shell_escape(char	*rest);
int insert_disk(char	*rest);

/* From cli.c */
void status(FILE *fp);
void cli(void);

/* From a2.c */
void run(void);

/* From table.c */
void flags(FILE *fp);
int diss(unsigned short pc, FILE *fp);

/* From main.c */
int bload(char *fnam, unsigned short addr);
void bsave(char *fnam, unsigned short addr, unsigned short size);
void set_term(void);
void restore_term(void);
void asfail( char	*file, int	line, char	*cond);

/* From screen.c */
void screen_setup(void);
void redraw_screen(void);

void set_text1f(unsigned short addr, unsigned char n);
#ifdef	TEXT2
void set_text2f(unsigned short addr, unsigned char n);
#endif	TEXT2

/* From prodos.c */
void prodos(void);

/* From dos.c */
void rwts(void);

/* From disk.c */
void read_disk(int track, int sector, unsigned char *buf);
void write_disk(int track, int sector, unsigned char *buf);

/* From vidout.c */
void scroll(void);
void vidout(void);

