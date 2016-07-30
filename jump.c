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
#import	<fcntl.h>
#import	<ctype.h>
#import	"a2.h"
#ifdef	JOYSTICK_SUPPORT
#import	<joystick.h>
#import	<errno.h>
#endif	JOYSTICK_SUPPORT

#define		jump_check(a,b,c,d)	(mem[Pc]==a && mem[Pc+1]==b && mem[Pc+2]==c && mem[Pc+3]==d)

/*
 *  In order to improve performance, we intercept the PC on JSR's
 *  and JMP's to certain locations and handled the intended function
 *  in C instead of letting the emulator interpret 6502.
 *
 *  This is done for video output, to produce an acceptable scroll;
 *  for the boot prom, so it crashes if there is no disk in the drive;
 *  and for other miscellaneous routines such as WAIT for speed.
 *
 *  In most cases the interceptor routine checks to see if the code it's
 *  intercepting looks like what it thinks should be there; it doesn't
 *  snatch the PC if the first four bytes of the routine don't match.
 */


#define		I_WAIT		1	/* defeat ROM WAIT routine           */
#define		I_PRODOS	2	/* Prodos high level intercept       */
#define		I_RWTS		3	/* DOS 3.3 high-level intercept      */
#define		I_BELL		4	/* don't toggle C030, output a bell  */
#define		I_VIDOUT	5	/* speeds up scrolling tremendously  */
#define		I_BOOT		6	/* crash if no disk on boot          */
#define		I_BOOTWAIT	7	/* defeat delay loops in DOS boot    */
#define		I_BOOTPATCH	8	/* patch dos for fast raw access     */
#ifdef	JOYSTICK_SUPPORT
#define		I_PREAD		9	/* patch dos for fast raw access     */
#endif	JOYSTICK_SUPPORT


extern int map_to_upper;
extern unsigned char disk_ref();

static int key_clear = TRUE;
static unsigned char last_key = 0;
static unsigned char temp_key;

void set_c0( unsigned short a, unsigned char n)
{
  
  switch (a & 0xFF) {
  case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
  case 0x16: case 0x17: case 0x18: case 0x19: case 0x1A: case 0x1B:
  case 0x1C: case 0x1D: case 0x1E: case 0x1F:
    key_clear = TRUE;
    last_key &= 0x7F;
    break;
    
  case 0x80: case 0x81: case 0x82: case 0x83:
  case 0x88: case 0x89: case 0x8A: case 0x8B:
    ram_card(a);
    break;
    
    /*
     *  Slot 6 Disk II memory map 
     */
    
#ifdef STUFFY_TURTLE
  case 0xC0E0: case 0xC0E1: case 0xC0E2: case 0xC0E3:
  case 0xC0E4: case 0xC0E5: case 0xC0E6: case 0xC0E7:
  case 0xC0E8: case 0xC0E9: case 0xC0EA: case 0xC0EB:
  case 0xC0EC: case 0xC0ED: case 0xC0EE: case 0xC0EF:
#endif
  case 0xE0: case 0xE1: case 0xE2: case 0xE3:
  case 0xE4: case 0xE5: case 0xE6: case 0xE7:
  case 0xE8: case 0xE9: case 0xEA: case 0xEB:
  case 0xEC: case 0xED: case 0xEE: case 0xEF:
    disk_ref(a, n);
    break;
    
  default:
    mem[a] = n;
  }
}

void set_special_jumps(void)
{
  mem[0x43] = 0x60;			/* for ProDos boot */
  
  jmp_tbl[0xBD00] = I_RWTS;
  jmp_tbl[0xC600] = I_BOOT;
  jmp_tbl[0xC680] = I_PRODOS;	/* patched into boot prom below */
  jmp_tbl[0x9D84] = I_BOOTPATCH;	/* fast raw dos access */
#ifdef	JOYSTICK_SUPPORT
  jmp_tbl[0xFB1E] = I_PREAD;
#endif	JOYSTICK_SUPPORT
  jmp_tbl[0xFBD9] = I_BELL;
  jmp_tbl[0xFBFD] = I_VIDOUT;
  jmp_tbl[0xFCA8] = I_WAIT;
  
  jmp_tbl[0x3A00] = I_BOOTWAIT;
  
  mem_set[0xC0] = set_c0;
  mem_set[0xC6] = set_writep;	/* write protect disk prom */
  
#if 0
  mem[0xC600] = 0;		/* flag for boot interception */
  mem[0xC601] = 0x20;		/* disk prom magic number */
  mem[0xC603] = 0x00;
  mem[0xC605] = 0x03;
  mem[0xC607] = 0x3C;
#endif
  
  /*
   *  Patch boot rom for fake Prodos driver
   */
  
  mem[0xC6FF] = 0x80;		/* C680 is driver address */
  mem[0xC6FE] = 0x1F;		/* info about device */
  
  screen_setup();
}


void jump(int key)
{
  switch (key) {
  case I_WAIT:					/* FCA8 */
    if (jump_check(0x38, 0x48, 0xE9, 0x01)) {
      A = 0;
      N = 0;
      V = 0;
      C = 1;
      DO_RTS;
    }
    break;
    
  case I_PRODOS:					/* C680 */
    prodos();
    break;
    
  case I_RWTS:					/* BD00 */
    if (jump_check(0x84, 0x48, 0x85, 0x49))
      rwts();
    break;
    
#ifdef	JOYSTICK_SUPPORT
  case I_PREAD:					/* FB1E */
    if (jump_check(0xAD, 0x70, 0xC0, 0xA0))
    {
	int my_x, my_y;
	struct JS_DATA_TYPE joystick_data;
	int status = read (joystick_fd, &joystick_data, JS_RETURN);
	if (status != JS_RETURN)
	{
	    int temperrno = errno;
	    perror ("js");
	    printf ("js: status = %d, errno = %d\n",
		    status, temperrno);
	    exit (1);
	}

	if (joystick_data.x <  xcenter)
	    my_x = (joystick_data.x - xmin) / xpartition_low;
	else  my_x = 128 + (joystick_data.x - xcenter) / xpartition_high;

	if (joystick_data.y <  ycenter)
	    my_y = (joystick_data.y - ymin) / ypartition_low;
	else  my_y = 128 + (joystick_data.y - ycenter) / ypartition_high;

	if (my_x < 0) my_x = 0;
	if (my_x > 255) my_x = 255;
	if (my_y < 0) my_y = 0;
	if (my_y > 255) my_y = 255;

	if (0 == X)  Y = my_x;
	if (1 == X)  Y = my_y;
    }
    break;    
#endif	JOYSTICK_SUPPORT

  case I_BELL:					/* FBD9 */
    if (jump_check(0x60, 0x87, 0xD0, 0x12)) {
      putchar(7);
      fflush(stdout);
      DO_RTS;
    }
    break;
    
  case I_VIDOUT:					/* FBFD */
    if (jump_check(0xC9, 0xA0, 0xB0, 0xEF))
      vidout();
    break;
    
  case I_BOOT:					/* C600 */
    if (disk[0] < 0) {
      info("boot: no disk");
      PCINC;			/* BRK into the monitor */
      push(high(Pc));
      push(low(Pc));
      B = 1;
      push(get_status());
      Pc = mem[0xFFFE] | (mem[0xFFFF] << 8);
      return;
    }
    info("boot");
    
    /* 
     *  We read the second half of a 512 byte block in case we're
     *  booting something that depends on this being a newer boot prom
     */
    
    drive = 0;
    read_disk(0, 14, &mem[0x900]);
    break;
    
    /*
     *  Standard DOS 3.3 has some pretty gross delay loops in its
     *  boot code.  The following patches defeat two of them.
     *  This could be dangerous; it seems to work, but DOS's original
     *  side effects are not maintained.  Comment out the jmp_tbl assignment
     *  of I_BOOTWAIT above if you are distrustful.
     *
     *  Interesting.  Dos relocates the patches when it moves into higher
     *  memory.  If you boot with a fast-booting dos that doesn't have the
     *  delays at 3A00, but still has them at BA00 & BD9E when it starts
     *  up, it will be slow if you turn off RWTS interception and use the
     *  raw interface.  However, slow-booting real DOS that gets patched
     *  while it's booting will have a faster raw interface, since it
     *  relocated the patches...
     */
    
  case I_BOOTWAIT:				/* 3A00 */
    if (jump_check(0xA2, 0x11, 0xCA, 0xD0)) {
      mem[0x3A00] = 0x60;		/* RTS */
      if (mem[0x3D9E] == 0xA0
	  &&  mem[0x3D9F] == 0x12
	  &&  mem[0x3DA0] == 0x88) {
	mem[0x3D9E] = 0x4C;	/* JMP past it */
	mem[0x3D9F] = 0xAB;
	mem[0x3DA0] = 0x3D;	/* gets relocated */
      }
    }
    break;
    
    /*
     *  This one is unnecessary since we do high-level RWTS interception
     */
    
  case I_BOOTPATCH:				/* 9D84 */
    if (jump_check(0xAD, 0xE9, 0xB7, 0x4A)) {
      if (mem[0xBA00] == 0xA2
	  &&  mem[0xBA01] == 0x11
	  &&  mem[0xBA02] == 0xCA) {
	mem[0xBA00] = 0x60;		/* RTS */
	if (mem[0xBD9E] == 0xA0
	    &&  mem[0xBD9F] == 0x12
	    &&  mem[0xBDA0] == 0x88) {
	  mem[0xBD9E] = 0x4C;
	  mem[0xBD9F] = 0xAB;
	  mem[0xBDA0] = 0xBD;
	}
      }
    }
    break;
    
  default:
    fprintf(stderr, "bad jump intercept key: %d\n", key);
    assert(FALSE);
  }
}


extern int save_flags;

unsigned char
  mem_map(a)
unsigned short a;
{
  
  switch (a) {
  case 0xC000:
#ifdef	KEYBOARD_BUFFER
   if (key_clear) {
#endif	KEYBOARD_BUFFER
      fcntl (0, F_SETFL, save_flags | O_NDELAY);
      
      if (read (0, &temp_key, 1) == 1) {
#ifdef	KEYBOARD_BUFFER
	key_clear = FALSE;
#endif	KEYBOARD_BUFFER
	if (temp_key == '\n')
	  temp_key = '\r';
	else if (temp_key == 127)
	  temp_key = 8;
	if (map_to_upper)
	  temp_key = toupper(temp_key);
	last_key = temp_key | 0x80;
      }
      
      fcntl (0, F_SETFL, save_flags);
#ifdef	KEYBOARD_BUFFER
    }
#endif	KEYBOARD_BUFFER
    return(last_key);
    
  case 0xC010:
#ifdef	KEYBOARD_BUFFER
    key_clear = TRUE;
#endif	KEYBOARD_BUFFER
    last_key &= 0x7F;
    return(0);			/* what should this be? */
    
  case 0xC011:
    if (bank2_enable)
      return(0xFF);
    return(0x00);
    
  case 0xC012:
    if (ram_read)
      return(0xFF);
    return(0x00);
    
  case 0xC054:
    screen_page = PRIMARY_PAGE;
    return(0x00);
    
  case 0xC055:
    screen_page = SECONDARY_PAGE;
    return(0x00);

#ifdef	JOYSTICK_SUPPORT

  case 0xC064:
	if (joystick_counter_x > 0)  return (0x80);
	else return (0x0);
	
  case 0xC065:
	if (joystick_counter_y > 0)  return (0x80);
	else return (0x0);

  case 0xC066:
  case 0xC067:
  case 0xC070:
    {
	int my_x, my_y;
	struct JS_DATA_TYPE joystick_data;
	int status = read (joystick_fd, &joystick_data, JS_RETURN);
	if (status != JS_RETURN)
	{
	    int temperrno = errno;
	    perror ("js");
	    printf ("js: status = %d, errno = %d\n",
		    status, temperrno);
	    exit (1);
	}

	if (joystick_data.x <  xcenter)
	    my_x = (joystick_data.x - xmin) / xpartition_low;
	else  my_x = 128 + (joystick_data.x - xcenter) / xpartition_high;

	if (joystick_data.y <  ycenter)
	    my_y = (joystick_data.y - ymin) / ypartition_low;
	else  my_y = 128 + (joystick_data.y - ycenter) / ypartition_high;

	if (my_x < 0) my_x = 0;
	if (my_x > 255) my_x = 255;
	if (my_y < 0) my_y = 0;
	if (my_y > 255) my_y = 255;

	if (0 == X)  Y = my_x;
	if (1 == X)  Y = my_y;
	joystick_counter_y = my_y * 4 + 1;
	joystick_counter_x = my_x * 4 + 1;
	return(0x00);
    }
#if 0
	MoveCursor(term_lines, 0);
	printf("Joystick Break at $%.4X\n", a);
//	tracing = FALSE;
	running = FALSE;
	return(0x00);
#endif

  case 0xC061:
    {
	struct JS_DATA_TYPE joystick_data;
	int status = read (joystick_fd, &joystick_data, JS_RETURN);
	if (status != JS_RETURN)
	{
	    int temperrno = errno;
	    perror ("js");
	    printf ("js: status = %d, errno = %d\n",
		    status, temperrno);
	    exit (1);
	}
	return((joystick_data.buttons & 1) ? 0x80 : 0x00);
    }

    
  case 0xC062:
    {
	struct JS_DATA_TYPE joystick_data;
	int status = read (joystick_fd, &joystick_data, JS_RETURN);
	if (status != JS_RETURN)
	{
	    int temperrno = errno;
	    perror ("js");
	    printf ("js: status = %d, errno = %d\n",
		    status, temperrno);
	    exit (1);
	}
	return((joystick_data.buttons & 2) ? 0x80 : 0x00);
    }

#endif	JOYSTICK_SUPPORT

  case 0xC080: case 0xC081: case 0xC082: case 0xC083:
  case 0xC088: case 0xC089: case 0xC08A: case 0xC08B:
    ram_card(a);
    return(0x00);
    
    /*
     *  Slot 6 Disk II memory map 
     */
    
  case 0xC0E0: case 0xC0E1: case 0xC0E2: case 0xC0E3:
  case 0xC0E4: case 0xC0E5: case 0xC0E6: case 0xC0E7:
  case 0xC0E8: case 0xC0E9: case 0xC0EA: case 0xC0EB:
  case 0xC0EC: case 0xC0ED: case 0xC0EE: case 0xC0EF:
    return( disk_ref(a, 0) );
    
#if 0
    /*
     *  Keep the boot prom magic number from appearing if there is
     *  no disk in the drive
     */
    
  case 0xC600:
  case 0xC601:
    if (disk[0] < 0)
      return(0);
    break;
#endif
    
  }
  
  return(mem[a]);		/* default */
}


