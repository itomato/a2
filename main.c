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
#import	<signal.h>
#import	<fcntl.h>
#import	"a2.h"


#ifdef	JOYSTICK_SUPPORT
int joystick_fd = -1;
int xmin = -1, ymin = -1;
int xcenter = -1, ycenter = -1;
float xpartition_low = 1, ypartition_low = 1;
float xpartition_high = 1, ypartition_high = 1;
int joystick_counter_x, joystick_counter_y;
#endif	JOYSTICK_SUPPORT

int save_flags;			/* terminal flags */
char escape_char = '~';

unsigned short Pc;
unsigned char Sp = 0xFF;	/* stack pointer 	*/
unsigned int  A = 0;		/* accumulator		*/
unsigned char X = 0;		/* X register		*/
unsigned char Y = 0;		/* Y register		*/

unsigned int N = 0;		/* 7 - sign		*/
unsigned int V = 0;		/* 6 - Overflow		*/
				/* 5 - Unused		*/
unsigned int B = 1;		/* 4 - Break		*/
unsigned int D = 0;		/* 3 - Decimal		*/
unsigned int I = 0;		/* 2 - Interrupt	*/
unsigned int NZ = 1;		/* 1 - Zero		*/
unsigned int C = 0;		/* 0 - Carry		*/

int term_lines, term_cols;
int running = TRUE;
int tracing = FALSE;

int disk[] = {-1, -1};
int write_prot[2];
int drive = 0;


void restore_term(void)
{
  
#ifdef STUFFY_TURTLE
  SetNormal();				/* Turn off inverse video */
#endif
  Raw(FALSE);
  fcntl(0, F_SETFL, save_flags);
  signal(SIGINT, SIG_DFL);
}

void asfail( char	*file, int	line, char	*cond)
{
  fprintf(stderr, "assertion failure: %s (%d): %s\n", file, line, cond);
  restore_term();
  abort();
  exit(1);
}

void cleanup(int signum)
{
  restore_term();
  exit(0);
}


void catch_intr(int signum)
{
  signal(SIGINT, catch_intr);
  running = FALSE;
  tracing = FALSE;
}

void set_term(void)
{
  signal(SIGINT, catch_intr);
  signal(SIGQUIT, cleanup);
  signal(SIGTERM, cleanup);
  save_flags = fcntl(0, F_GETFL, 0);
  Raw(TRUE);
}

int bload(char *fnam, unsigned short addr)
{
  int fd;
  long len;
  
  if ((fd = open(fnam, O_RDONLY)) < 0) {
    fprintf(stderr, "can't open %s: ", fnam);
    perror("");
    return(FALSE);
  }
  
  len = 65536 - addr;
  
  if (len == 65536) {		/* stupid $%!*&#~ 16 bit systems */
    if (read(fd, &mem[addr], 4096) < 0) {
      fprintf(stderr, "bad read of %s: ", fnam);
      perror("");
      return(FALSE);
    }
    
    addr += 4096;
    len -= 4096;
  }
  
  if (read(fd, &mem[addr], len) < 0) {
    fprintf(stderr, "bad read of %s: ", fnam);
    perror("");
    return(FALSE);
  }
  
  close(fd);
  return(TRUE);
}

void bsave(char *fnam, unsigned short addr, unsigned short size)
{
  int fd;
  unsigned sizel;
  int x;
  sizel=size;
  if (sizel==0) sizel=65535;
  if ((fd=open(fnam,O_WRONLY|O_CREAT,0644)) < 0) {
    printf("can't open %s: ", fnam);
    perror("");
    return;
  }
  x=write(fd,&mem[addr],sizel);
  if (size == 0)
    x=write(fd,&mem[65535],1);
  if (x==-1) perror("write");
  close(fd);
}


/*
 *  I make certain assumptions about rollover so that I can do
 *  things like X++ and know that if X is 0xFF it will rollover
 *  to 0x00.  If I didn't know this I'd have to do X = (X+1) & 0xFF
 *  If your machine assert fails on the code below you'll have to
 *  rewrite the code that depends on rollover
 */

void safety_check(void)
{
  unsigned char c;
  unsigned short s;
  
  c = 0xFF;
  assert(++c == 0x00);
  assert(--c == 0xFF);
  
  s = 0xFFFF;
  assert(++s == 0x0000);
  assert(--s == 0xFFFF);
}


void simulator_main(int argc,char **argv)
{
  int c;
  int errflag = 0;
  char *f8rom, *d0rom;
  int cli_first = FALSE;
  char *startup_disk = "Samp.disk";

  safety_check();
  f8rom = "AUTOSTART.ROM";
  d0rom = "APPLESOFT.ROM";
  
  while ((c = getopt(argc, argv, "cimd:")) != -1) {
    switch (c) {
    case 'c':
      cli_first = TRUE;
      break;
      
    case 'i':
      d0rom = "INTEGER.ROM";
      break;
      
    case 'm':
      f8rom = "MONITOR.ROM";
      break;
      
    case 'd':
      startup_disk = optarg;
      break;
      
    case '?':
    default:
      errflag++;
    }
  }
  
  if (errflag) {
    fprintf(stderr, "usage:  %s\n", argv[0]);
    fprintf(stderr, "\t-m\tLoad MONITOR.ROM instead of AUTOSTART.ROM at $F800\n");
    fprintf(stderr, "\t-i\tLoad INTEGER.ROM instead of APPLESOFT.ROM at $D000\n");
    fprintf(stderr, "\t-c\tEnter command mode before executing any instructions\n");
    fprintf(stderr, "\t-d <FILE>\tInsert <FILE> as disk 1 on startup\n");
    exit(1);
  }
  
  printf("a2 -- Apple II emulator.  Escape character is %c\n",
	 escape_char);
  
  if (!bload(f8rom, 0xF800))
    exit(1);
  bload(d0rom, 0xD000);
  bload("DISK.PROM", 0xC600);
  
  if (!InitScreen())
    exit(1);
  ScreenSize(&term_lines, &term_cols);
  
  memory_setup();
  set_special_jumps();
  Pc = join(mem[0xFFFC], mem[0xFFFD]);

  /* mjhostet HACK */
  insert_disk (startup_disk);

#ifdef	JOYSTICK_SUPPORT
    /* open device file */
    joystick_fd = open ("/dev/js0", O_RDONLY);
    if (joystick_fd < 0)
    {
	perror ("a2 - open joystick");
	exit (1);
    }

#endif	JOYSTICK_SUPPORT

  if (cli_first)
    cli();
  else
    ClearScreen();
  while (1) {
    set_term();
    run();
    cli();
  }
}
