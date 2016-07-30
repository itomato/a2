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



/*
 *  Return codes for match(), find_box(), and parse()
 */

#define		NO_MATCH	-1	/* strings are different */
#define		PARTIAL		-3	/* partial match */
#define		IDENTICAL	-4	/* strings matched perfectly */

#define		AMBIGUOUS	-2	/* amiguous match */
#define		OK		-5	/* command executed ok */
#define		NOT_IMPLEMENTED	-6	/* haven't written this one yet */
#define		DISPLAY		-7	/* redisplay the diss & flags line */

#define MAXSTACK 100

struct cmdtbl {
	char *name;
	int (*func)();
};

extern long breakpoint;
extern struct cmdtbl first_tbl[];
extern int in_cli;
extern long trace_lo, trace_hi;
extern long get_hex_number();
extern FILE *logging_fp;
extern char *getenv();
extern char *split();

