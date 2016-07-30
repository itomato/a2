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
#import	<setjmp.h>
#import	<signal.h>
#import	"a2.h"
#import	"cli.h"


FILE *logging_fp = NULL;
long	breakpoint = -1;
long	trace_lo = -1;
long	trace_hi;
int	in_cli;

unsigned short lpoint;		/* point where dissassembly occurs */
long phantom_location = -1;
int map_to_upper = 1;
jmp_buf jb;

void status(FILE *fp)
{
  diss(Pc, fp);
  flags(fp);
}

void cli_catch(int signum)
{
	signal(SIGINT,cli_catch);
	printf("\n");
	longjmp(jb,1);
}

void cli(void)
{
	int parse(struct cmdtbl tbl[], char *s);
	char	foo[200];

	restore_term();
	MoveCursor(term_lines, 0);
	status(stdout);
	in_cli = TRUE;
	lpoint = Pc;
	signal(SIGINT, cli_catch);
	setjmp(jb);
	do {
		printf(">>>");
		if (fgets(foo, 200, stdin) == NULL) {
			printf("\n");
			exit(0);
		}
		foo[strlen(foo)-1] = '\0';
		if (parse(first_tbl, foo)) {
			running = FALSE;
			tracing = FALSE;
			return;			/* single step; no redraw */
		}
	} while (in_cli);

	set_term();
	redraw_screen();
	if (breakpoint != -1 || trace_lo != -1) {
		tracing = TRUE;
		running = FALSE;
	} else {
		tracing = FALSE;
		running = TRUE;
	}
}
