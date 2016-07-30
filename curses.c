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
**  This a screen management library borrowed with permission from the
**  Elm mail system (a great mailer--I highly recommend it!).
**
**  I've hacked this library to only provide what I need.
**
**  Original copyright follows:
*/
/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 2.1 $   $State: Exp $
 *
 * 			Copyright (c) 1986 Dave Taylor
 ******************************************************************************/

#import <stdio.h>
#import <libc.h>
#import <c.h>
#import <strings.h>
#import <ctype.h>

#define DEFAULT_term_lines	24
#define DEFAULT_COLUMNS	80
#define TTYIN	0


#define		VERY_LONG_STRING	2500

int term_lines = DEFAULT_term_lines - 1;
int term_cols  = DEFAULT_COLUMNS;
extern char escape_char;

#ifdef SHORTNAMES
# define _cleartoeoln	_clrtoeoln
# define _cleartoeos	_clr2eos
#endif

#define TCGETA	TIOCGETP
#define TCSETAW	TIOCSETP

#ifdef NeXT
struct tchars	raw_tchars,
		orig_tchars;
#endif

struct sgttyb raw_tty,
	      orig_tty;

static int inraw = 0;                  /* are we IN rawmode?    */


#ifdef STUFFY_TURTLE
static int _memory_locked = 0;		/* are we IN memlock??   */

static int _intransmit;			/* are we transmitting keys? */
#endif

static
char *_clearscreen, *_moveto, *_cleartoeoln, *_cleartoeos,
	*_setinverse, *_clearinverse;


static
int _lines,_columns;

static char _terminal[1024];              /* Storage for terminal entry */
static char _capabilities[1024];           /* String for cursor motion */

static char *ptr = _capabilities;	/* for buffering         */

void tputs(register char *cp, int affcnt, int (*outc)(char));
char  *tgetstr(),     		       /* Get termcap capability */
      *tgoto();				/* and the goto stuff    */
int tgetnum();     		       /* Get termcap capability */
int  tgetent();				/* get termcap entry */

int myoutchar(char c)
{
  /** output the given character.  From tputs... **/
  /** Note: this CANNOT be a macro!              **/

  return putc(c, stdout);
}

int InitScreen(void)
{
int     err;
char termname[40];
	
	if (getenv("TERM") == NULL) {
		fprintf(stderr,
		  "TERM variable not set; Screen capabilities required\n");
		return(FALSE);
	}
	if (strcpy(termname, getenv("TERM")) == NULL) {
		fprintf(stderr,"Can't get TERM variable\n");
		return(FALSE);
	}
	if ((err = tgetent(_terminal, termname)) != 1) {
		fprintf(stderr,"Can't get entry for TERM\n");
		return(FALSE);
	}

	/* load in all those pesky values */
	_clearscreen       = tgetstr("cl", &ptr);
	_moveto            = tgetstr("cm", &ptr);
	_cleartoeoln       = tgetstr("ce", &ptr);
	_cleartoeos        = tgetstr("cd", &ptr);
	_setinverse        = tgetstr("so", &ptr);
	_clearinverse      = tgetstr("se", &ptr);
	_lines	      	   = tgetnum("li");
	_columns	   = tgetnum("co");

	if (!_clearscreen) {
		fprintf(stderr,
			"Terminal must have clearscreen (cl) capability\n");
		return(FALSE);
	}
	if (!_moveto) {
		fprintf(stderr,
			"Terminal must have cursor motion (cm)\n");
		return(FALSE);
	}
	if (!_cleartoeoln) {
		fprintf(stderr,
			"Terminal must have clear to end-of-line (ce)\n");
		return(FALSE);
	}
	if (!_cleartoeos) {
		fprintf(stderr,
			"Terminal must have clear to end-of-screen (cd)\n");
		return(FALSE);
	}
	if (_lines == -1)
		_lines = DEFAULT_term_lines;
	if (_columns == -1)
		_columns = DEFAULT_COLUMNS;
	return(TRUE);
}

void ScreenSize(int *lines, int *columns)
{
	/** returns the number of lines and columns on the display. **/

	if (_lines == 0) _lines = DEFAULT_term_lines;
	if (_columns == 0) _columns = DEFAULT_COLUMNS;

	*lines = _lines - 1;		/* assume index from zero*/
	*columns = _columns;		/* assume index from one */
}

void ClearScreen(void)
{
	/* clear the screen: returns -1 if not capable */

	tputs(_clearscreen, 1, myoutchar);
	fflush(stdout);      /* clear the output buffer */
}

void MoveCursor(int row, int col)
{
	/** move cursor to the specified row column on the screen.
            0,0 is the top left! **/

	char *stuff, *tgoto();

	stuff = tgoto(_moveto, col, row);
	tputs(stuff, 1, myoutchar);
/*	fflush(stdout);	*/
}

void CleartoEOLN(void)
{
	/** clear to end of line **/

	tputs(_cleartoeoln, 1, myoutchar);
	fflush(stdout);  /* clear the output buffer */
}

void CleartoEOS(void)
{
	/** clear to end of screen **/

	tputs(_cleartoeos, 1, myoutchar);
	fflush(stdout);  /* clear the output buffer */
}

void Raw(int state)
{
    static int firstime_Raw = 0;
	/** state is either TRUE or FALSE, as indicated by call **/

	if (state == FALSE && inraw) {
	  (void) ioctl(TTYIN, TCSETAW, &orig_tty);
	  (void) ioctl(TTYIN, TIOCSETC, &orig_tchars);
	  inraw = 0;
	}
	else if (state == TRUE && ! inraw) {

	  (void) ioctl(TTYIN, TCGETA, &orig_tty);	/** current setting **/

	  (void) ioctl(TTYIN, TCGETA, &raw_tty);    /** again! **/
#ifdef STUFFY_TURTLE
	  raw_tty.sg_flags &= ~(ECHO | CRMOD);	/* echo off */
#endif
	  raw_tty.sg_flags &= ~ECHO;		/* echo off */
	  raw_tty.sg_flags |= CBREAK;	/* raw on    */

#ifndef NeXT
	  raw_tty.c_cc[VINTR]= escape_char;
#else	NeXT
	    if (!firstime_Raw)
	    {
		firstime_Raw = 1;
		(void) ioctl(TTYIN, TIOCGETC, &orig_tchars);
	    }
	    raw_tchars = orig_tchars;
	    raw_tchars.t_intrc = escape_char;
	    (void) ioctl(TTYIN, TIOCSETC, &raw_tchars);
#endif
	  (void) ioctl(TTYIN, TCSETAW, &raw_tty);
	  inraw = 1;
	}
}

int
ReadCh(void)
{
	/** read a character with Raw mode set! **/

	register int result;
	char ch;
	result = read(0, &ch, 1);
        return((result <= 0 ) ? EOF : ch);
}


#if 0
static int inverse = FALSE;

SetInverse() {

	if (!inverse) {
		StartInverse();
		inverse = TRUE;
	}
}

SetNormal() {

	if (inverse) {
		EndInverse();
		inverse = FALSE;
	}
}
#endif


void StartInverse(void)
{
	/** set inverse video mode **/

	if (!_setinverse)
		return;

	tputs(_setinverse, 1, myoutchar);
/*	fflush(stdout);	*/
}


void EndInverse(void)
{
	/** compliment of startinverse **/

	if (!_clearinverse)
		return;

	tputs(_clearinverse, 1, myoutchar);
/*	fflush(stdout);	*/
}
