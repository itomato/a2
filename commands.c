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
#import	<ctype.h>
#import	<signal.h>
#import	"a2.h"
#import	"cli.h"
#ifdef	JOYSTICK_SUPPORT
#import	<joystick.h>
#import	<errno.h>
#endif	JOYSTICK_SUPPORT


extern unsigned short lpoint;
extern long phantom_location;
extern char *getcwd();
extern int map_to_upper;
extern char escape_char;
long get_hex_number();

char diskname[2][200];		/* disk names stored here */

struct point_stack {
	unsigned short data[MAXSTACK];
	int sp;
} pstack;

void init_point(void)
{
	pstack.sp = -1;
}

int pop_point(char *rest)
{
	if (pstack.sp < 0 ) {
		printf("stack empty\n");
		return OK;
	}
	switch (*rest){
		case 'l':
		case 'L':
			printf("%x\n", lpoint = pstack.data[pstack.sp--]);
			break;
		case 'p':
		case 'P':
			printf("%x\n", lpoint = pstack.data[pstack.sp--]);
			break;
		default : 
			printf("pop [lp]\n");
			break;
	}
	return(OK);
}

int dup_point(char *rest)
{
	if (pstack.sp < 0 ) {
		printf("stack empty\n");
		return OK;
	}
	switch (*rest){
		case 'l':
		case 'L':
			lpoint = pstack.data[pstack.sp];
			break;
		case 'p':
		case 'P':
			lpoint = pstack.data[pstack.sp];
			break;
		default : 
			printf("dup [lp]\n");
			break;
	}
	return(OK);
}

int push_point(char *rest)
{
	long value;
	char *addr;

	assert(pstack.sp < MAXSTACK);
	switch (*rest){
		case 'l':
		case 'L':
			pstack.data[++pstack.sp] = lpoint;
			break;
		case 'p':
		case 'P':
			pstack.data[++pstack.sp] = Pc;
			break;
		default:
			addr = rest;
			rest = split(rest);
			value = get_hex_number(addr);
			if (value == -1L) 
				printf("push [l|p|<addr>]\n");
			else 
				pstack.data[++pstack.sp]=(unsigned short)value;
			break;
	}
	return(OK);
}


int clb(char *junk)
{

	B = 0;
	return(DISPLAY);
}


int seb(char *junk)
{

	B = 1;
	return(DISPLAY);
}


int clc(char	*junk)
{
	C = 0;
	return(DISPLAY);
}


int sec(char	*junk)
{
	C = 1;
	return(DISPLAY);
}


int sev(char	*junk)
{
	V = 1;
	return(DISPLAY);
}


int clv(char	*junk)
{
	V = 0;
	return(DISPLAY);
}


int sed(char	*junk)
{
	D = 1;
	return(DISPLAY);
}


int cld(char *junk)
{
	D = 0;
	return(DISPLAY);
}


int sei(char	*junk)
{
	I = 1;
	return(DISPLAY);
}


int clri(char	*junk)
{
	I = 0;
	return(DISPLAY);
}


int sen(char	*junk)
{
	N = 1;
	return(DISPLAY);
}


int cln(char	*junk)
{
	N = 0;
	return(DISPLAY);
}


int sez(char	*junk)
{
	NZ = 0;
	return(DISPLAY);
}


int clz(char	*junk)
{
	NZ = 1;
	return(DISPLAY);
}


void quit_emulator(char	*junk)
{
  restore_term();
  exit(0);
}



int ver(char	*junk)
{
	printf("a2 - Apple II emulator  (c) Copyright 1990 by Rich Skrenta & Tom Markson\n");
	return(OK);
}


int refresh(char	*junk)
{

	in_cli = FALSE;
	return(OK);
}


int shell_escape(char	*rest)
{
	char	line[100];
	char *s;

	s = getenv("SHELL");
	if (s == NULL)
		s = "/bin/sh";

	strcpy(line, s);

	if (*rest != '\0') {
		strcat(line, " -c '");
		strcat(line, rest);
		strcat(line, "'");
	}
	system(line);

	printf("\n");
	return(OK);
}


int do_soft_reset(char	*rest)
{

	Pc = mem[0xFFFC] | (mem[0xFFFD] << 8);
	return(DISPLAY);
}


int do_bload(char	*rest)
{
	char	*first;
	char	*file;
	unsigned short	start;
	long	foo;

	file = rest;
	rest = split(rest);
	first = rest;
	rest = split(rest);
	foo = get_hex_number(first);
	if (foo == -1) {
		printf("usage: bload file hex-address\n");
		return(OK);
	}
	start = (unsigned int) foo;
	bload(file, start);

	return(OK);
}


int do_bsave(char	*rest)
{
	char	*startc, *sizec, *file;
	unsigned short	start, size;
	long	istart, iend;
	file = rest;
	rest = split(rest);
	startc = rest;
	rest = split(rest);
	sizec = rest;
	rest = split(rest);
	istart = get_hex_number(startc);
	iend = get_hex_number(sizec);
	if ((istart == -1) || (iend == -1))
		printf("usage: bsave file hex-address hex-length\n");
	else {
		start = (unsigned short) istart;
		size  = (unsigned short) iend;
		bsave(file, start, size);
	}

	return(OK);
}



int show_point(char	*rest)
{

	lpoint = Pc;
	return(DISPLAY);
}


int hack(char *rest)
{
extern int cur_track;

	cur_track = get_hex_number(rest);
	return(OK);
}


int do_jump(char	*rest)
{
	char	*start;
	long	istart;
	start = rest;
	rest = split(rest);
	istart = get_hex_number(start);

	if (istart == -1) {
		printf("usage: jmp <hex address>\n");
		return(OK);
	} else {
		Pc = istart & 0xFFFF;
		return(DISPLAY);
	}
}



int trace(char	*rest)
{
	char	*addr1, *addr2, *file;
	long	addr1i, addr2i;

	addr1 = rest;
	rest = split(rest);
	addr2 = rest;
	rest = split(rest);
	file = rest;
	rest = split(rest);
	addr1i = get_hex_number(addr1);
	addr2i = get_hex_number(addr2);
	if (addr1i == -1 && addr2i == -1) {
		if (trace_lo == -1)
			printf("No trace region set\n");
		else
			printf("Tracing between $%.4X and $%.4x\n",
				(unsigned int)trace_lo,
				(unsigned int)trace_hi);
		return(OK);
	}

	if (addr1i == -1 || addr2i == -1) {
		printf("usage: trace [low high]\n");
		return(OK);
	}

	if (logging_fp == NULL) {
		if (*file == '\0' || file == NULL) {
			printf("Trace log will go to file 'trace'.\n");
			file = "trace";
		}
		logging_fp = fopen(file, "w");
		if (logging_fp == NULL) {
			perror("can't open trace file");
			trace_lo = -1;
			return(OK);
		}
	}

	trace_lo = addr1i & 0xFFFF;
	trace_hi = addr2i & 0xFFFF;

	return(OK);
}


int ldx(char	*rest)
{
	long	number;
	char	*first;

	first = rest;
	rest = split(rest);
	number = get_hex_number(first);

	number &= 0xFF;
	if (number < 0) {
		printf("usage: ldx <hex number>\n");
		return(OK);
	}

	X = number & 0xFF;
	return(DISPLAY);
}


int ldy(char	*rest)
{
	long	number;
	char	*first;
	first = rest;
	rest = split(rest);
	number = get_hex_number(first);

	if (number < 0) {
		printf("usage: ldy <hex number>\n");
		return(OK);
	}

	Y = number & 0xFF;
	return(DISPLAY);
}


int lda(char	*rest)
{
	long	number;
	char	*first;
	first = rest;
	rest = split(rest);
	number = get_hex_number(first);
	if (number < 0) {
		printf("usage: lda <hex number>\n");
		return(OK);
	}

	A = number & 0xFF;
	return(DISPLAY);
}


int lds(char	*rest)
{
	long	number;
	char	*first;
	first = rest;
	rest = split(rest);
	number = get_hex_number(first);
	if (number < 0) {
		printf("usage: lds <hex number>\n");
		return(OK);
	}

	Sp = number & 0xFF;
	return(DISPLAY);
}


int set_break_point(char	*rest)
{	
	long	addr;
	char	*first;
	first = rest;
	rest = split(rest);
	addr = get_hex_number(first);
	if (addr == -1)
		if (breakpoint == -1)
			printf("no breakpoint set\n");
			else
			printf("break point set at %x\n",
			    (unsigned short)breakpoint);
			else
		breakpoint = addr;
	running = FALSE;
	return(OK);
}


int clear_break_point(char	*rest)
{
	breakpoint = -1;
	return(OK);
}


int notrace(char	*junk)
{

	trace_lo = -1;
	if (logging_fp == NULL)
		return(OK);
	else
		fclose(logging_fp);
	logging_fp = NULL;
	return(OK);
}


int insert_disk(char *rest)
{
	char	*name;
	char	*which;
	int	fd;
	int	read_only = 0;

	name = rest;
	rest = split(rest);
	which = rest;
	rest = split(rest);

	if (name == NULL || *name == '\0') {
		printf("usage: insert <file name> [drive]\n");
		return(OK);
	}

	fd = open(name, 2);		/* attempt open for read/write */
	if (fd >= 0)
		read_only = 0;
	else  {				/* attempt open read only */
		read_only = 1;
		fd = open(name, 0);
	}
	if (fd < 0) {
		fprintf(stderr, "can't open %s: ", name);
		perror("");
		return(OK);
	}

	if (*which == '2')
		drive = 1;
	else
		drive = 0;

	if (disk[drive] >= 0)
		close(disk[drive]);

	strcpy(diskname[drive], name);

	disk[drive] = fd;
	write_prot[drive] = read_only;

	printf("disk %d inserted, %swrite protected\n", drive + 1,
				read_only ? "" : "not ");

	return(OK);
}


int dissassemble(char *rest)
{
	long istart;
	long iend;
	int count = 0;
	char *first,*last;

	first = rest;
	rest = split(rest);
	last = rest;
	rest = split(rest);

	istart = get_hex_number(first);
	iend   = get_hex_number(last);
	if (istart != -1)
		lpoint = istart;
	if (iend == -1)
		iend=65537;
	while ( (long) lpoint < iend) {
		lpoint += diss(lpoint, stdout);
		printf("\n");
		count++;
		if (iend == 65537)
			if (count > term_lines-3)
				break;
	}
	return OK;
}

void ascii_dump(unsigned short l, unsigned short h)
{
	while (l < h) {
		if (isprint(mem[l]))
			printf("%c",mem[l]);
		else 
			printf(".");
		l++;
	}
}

int hex_dump(char *rest)
{
	char *start,*end;
	unsigned short last, addr,oaddr;
	long iaddr1,iaddr2;
	int count;

	start = rest;
	rest = split(rest);
	end = rest;
	rest = split(rest);
	iaddr1 = get_hex_number( start );
	iaddr2 = get_hex_number( end );

	if (iaddr2 != -1 && iaddr1 > iaddr2)
		return(OK);

	if (iaddr1 != -1)
		lpoint = (unsigned short) iaddr1;
	if (iaddr2 == -1)
		last = lpoint + 1;
	else
		last = (unsigned short) iaddr2 + 1;

	last &= 0xFFFF;

	addr = lpoint;
	count = 0;
	printf("%.4X:  ", addr);
	oaddr = addr;
	do {
		if (count % 16 == 0 && count != 0) {
			ascii_dump(oaddr,addr);
			oaddr = addr;
			printf("\n%.4X:  ", addr);
		}
		printf("%.2X ", mem[addr]);
		addr++;
		count++;
	} while (addr != last);
	while ((count % 16) != 0) {
		printf("   ");  /* 3 spaces dd_ */
		count++;
	}
	ascii_dump(oaddr,addr);
	printf("\n");
	return(OK);
}

int deposit(char *rest)
{
	char *addr;
	char *value;
	unsigned short location;
	long iloc;
	long pre_val;
	unsigned char val;
	int fired_once;

	addr = rest;
	rest = split(rest);
	fired_once = 0;
	iloc = get_hex_number(addr);
	if (iloc == -1) {
		printf("usage: deposit <addr> <value> [<value>...]\n");
		return(OK);
	}

	location = (unsigned short) iloc;
	do {
		value = rest;
		rest = split(rest);
		pre_val = get_hex_number(value);
		if (pre_val == -1) {
			if (!fired_once)
				printf("Invalid or Missing Hex address\n");
			return OK;
		}
		else val = pre_val;
		mem[location++] = val;
		fired_once = 1;
	} while (*rest != '\0');

	return(OK);
}

int phantom_trace(char *rest)
{
	char *phantoms;
	char *addr1s;
	char *addr2s;
	char *file;
	long  phantom;
	long low_val,high_val;

	phantoms = rest;
	rest = split(rest);
	addr1s = rest;
	rest = split(rest);
	addr2s = rest;
	rest = split(rest);
	file = rest;
	rest = split(rest);

	phantom = (unsigned short)get_hex_number(phantoms);
	low_val = get_hex_number(addr1s);
	high_val = get_hex_number(addr2s);
	if (*phantoms == '\0') {
		if (phantom_location == -1) {
			printf("The phantom sleeps.");
			if (trace_lo != -1 && trace_hi != -1)
				printf("however, a trace is active.");
			printf("\n");
		} else 
		printf("the phantom waits until Pc = %.4X and then traces from %.4X to %.4X\n",
			(unsigned int)phantom_location,
			(unsigned int)trace_lo,
			(unsigned int)trace_hi);
		return(OK);
	}
	if (low_val == -1 || high_val == -1 || phantom == -1) {
		printf("usage: phantom <addr> <addr> <addr> [file]\n");
		return OK;
	} phantom_location = phantom; trace_lo = low_val; trace_hi = high_val; if (logging_fp == NULL) {
		if (*file == '\0' || file == NULL) {
			printf("the phantom will trace to file 'trace'.\n");
			file = "trace";
		}
		logging_fp = fopen(file, "w");
		if (logging_fp == NULL) {
			perror("can't open trace file");
			trace_lo = -1;
			return(OK);
		}
	}
	return OK;
}

int no_phantom_trace(char *rest)
{
	phantom_location = -1;
	trace_lo = -1;
	printf("the phantom goes to sleep.\n");
	if (logging_fp == NULL)
		return OK;
	fclose(logging_fp);
	logging_fp = NULL;
	return OK;
}

int cd(char *rest)
{

	char *first;
	char path[200];

	first = rest;
	rest = split(rest);
	if (*first != '\0') {
		if (chdir(first)) {
			perror("cd");
			printf("CWD remains ");
		}
	}
#ifdef NeXT
	printf("%s\n",getwd(path));
#else
	printf("%s\n",getcwd(path,198));
#endif

	return OK;
}

int map(char *rest)
{
	map_to_upper = !map_to_upper;
	printf("Uppercase Mapping is %s\n",(map_to_upper)?"On":"Off");
	return OK;
}

int sex(char *rest)
{
	printf("You'll need a real Apple for that sort of thing\n");
	return OK;
}


int help(char *rest)
{
	printf("![command]		Escape to Unix\n");
	printf(".			Display Current Pc Point\n");
	printf("bload file addr		load binary file into mem at addr\n");
	printf("breakpoint [addr]	Set the Breakpoint to addr\n");
	printf("bsave file start end	Save Memory from start to end in file\n");
	printf("cd [directory]		Set/Show Current Working Directory\n");
	printf("cl[cdinvz]		Clear appropriate Status Bit\n");
	printf("continue		Resume Execution of Emulator\n");
	printf("deposit addr [val]+	Put val(s) into addr\n");
	printf("dup [l|p]		duplicate top of stack into l or p\n");
	printf("escape char		set escape char to be char \n");
	printf("examine addr		Display Value in addr\n");
	printf("insert file drive#	Make file disk in drive#\n");
	printf("jmp addr		Make Pc=addr\n");
	printf("ld[asxy] val		Load Register with val\n");
	printf("list [addr] [addr]	Dissassemble at point or addr\n");
	printf("map			Toggle lower -> upper case mapping\n");
	printf("nobreak			Turn off breakpoint\n");
	printf("pop [l|p]		get p or l from top of stack\n");
	printf("push [l|p|<addr>	push l,p, or hex addr on stack\n");
	printf("reset			Pc = Apple Reset Vector\n");
#ifdef	JOYSTICK_SUPPORT
	printf("recalibrate		Recalibrate joystick\n");
#endif	JOYSTICK_SUPPORT
	printf("se[cdinvz]		Set appropriate Status Flag\n");
	printf("trace [addr] [addr]	Trace Between addresses/display trace point\n");
	return OK;
}

int set_escape_char(char *rest)
{
	if (*rest != '\0')
		escape_char = *rest;
	printf("escape character is ");
	if (isprint(escape_char))
		printf("%c",(int)escape_char);
	else 
		printf("^%c",(char)escape_char+64);
	printf(" (0x%.2X)",(int)escape_char);
	printf("\n");
	return(OK);
}

int disk_names(char *rest)
{

	printf("drive 1: %s\n", disk[0] >= 0 ? diskname[0] : "empty");
	printf("drive 2: %s\n", disk[1] >= 0 ? diskname[1] : "empty");
	return(OK);
}

#ifdef	JOYSTICK_SUPPORT
int do_joystick_recalibrate(char *rest)
{
    int status;
    struct JS_DATA_TYPE js;
    int xmax, ymax;

    printf ("Joystick calibration:\n");
    printf ("Move joystick to upper left corner and press button one.\n");
    while ((read (joystick_fd, &js, JS_RETURN) > 0) && (!(js.buttons & 1))) 
	;

    status = read (joystick_fd, &js, JS_RETURN);
    if (status != JS_RETURN)
    {
	int temperrno = errno;
	perror ("js");
	printf ("js - status = %d, errno = %d\n",
		status, temperrno);
	exit (1);
    }

    xmin = js.x;
    ymin = js.y;

    printf ("Move joystick to lower right corner and press button two.\n");
    while ((read (joystick_fd, &js, JS_RETURN) > 0) && (!(js.buttons & 2)))
	;

    status = read (joystick_fd, &js, JS_RETURN);
    if (status != JS_RETURN)
    {
	int temperrno = errno;
	perror ("js");
	printf ("js - status = %d, errno = %d\n",
		status, temperrno);
	exit (1);
    }

    xmax = js.x;
    ymax = js.y;

    printf ("Center joystick and press button one\n");
    while ((read (joystick_fd, &js, JS_RETURN) > 0) && (!(js.buttons & 1)))
	;

    status = read (joystick_fd, &js, JS_RETURN);
    if (status != JS_RETURN)
    {
	int temperrno = errno;
	perror ("js");
	printf ("js - status = %d, errno = %d\n",
		status, temperrno);
	exit (1);
    }

    xcenter = js.x;
    ycenter = js.y;

    if (((xmax - xcenter) < 128)
    || ((xcenter - xmin) < 128))
    {
	printf ("X-axis calibration failed\n");
	return OK;
    }

    if (((ymax - ycenter) < 128)
    || ((ycenter - ymin) < 128))
    {
	printf ("Y-axis calibration failed\n");
	return OK;
    }

    xpartition_low = ((float)(xcenter - xmin)) / 128.0;
    ypartition_low = ((float)(ycenter - ymin)) / 128.0;

    xpartition_high = ((float)(xmax - xcenter)) / 128.0;
    ypartition_high = ((float)(ymax - ycenter)) / 128.0;
    
    js.buttons = 0;
    
    printf ("Press both buttos to end recalibration testing.\n");
    while ((!(js.buttons & 1)) || (!(js.buttons & 2)))
    {

	int my_x, my_y;
	
	status = read (joystick_fd, &js, JS_RETURN);
	if (status != JS_RETURN)
	{
	    int temperrno = errno;
	    perror ("js");
	    printf ("js - status = %d, errno = %d\n",
		    status, temperrno);
	    exit (1);
	}

	if (js.x <  xcenter)  my_x = (js.x - xmin) / xpartition_low;
	else  my_x = 128 + (js.x - xcenter) / xpartition_high;

	if (js.y <  ycenter)  my_y = (js.y - ymin) / ypartition_low;
	else  my_y = 128 + (js.y - ycenter) / ypartition_high;

	if (my_x < 0) my_x = 0;
	if (my_x > 255) my_x = 255;
	if (my_y < 0) my_y = 0;
	if (my_y > 255) my_y = 255;

	fprintf (stdout, "button 0: %s  button 1: %s  X position: %4d  Y position: %4d\r",
		(js.buttons & 1) ? "on " : "off",
		(js.buttons & 2) ? "on " : "off",
		my_x, my_y);
		
	fflush (stdout);

	/* give other processes a chance */
	usleep (100);
    }

    return OK;
}
#endif	JOYSTICK_SUPPORT

