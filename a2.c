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


#include	<stdio.h>
#include	"a2.h"

#define	BRANCH()	val = mem[Pc++]; if (val & 0x80) Pc += val - 256; else Pc += val;

#define	put_status(s) stmp = s; N = stmp & 0x80; V = stmp & 0x40; B = stmp & 0x10; D = stmp & 0x08; I = stmp & 0x04; NZ = !(stmp & 0x02); C = stmp & 0x01;


extern long trace_lo, trace_hi;
extern FILE *logging_fp;
extern long phantom_location;
extern long breakpoint;

void redraw_hgr_screen (unsigned char *a2buf);

void run(void)
{
  register unsigned short ptr;
  register unsigned short ptmp;
  register unsigned int val;
  register unsigned int tmp;
  register unsigned int num;
  unsigned char stmp;
  int foo;
  int show_flags = FALSE;
  

  unsigned long mjhostet = 0;


  do {					/* while (tracing) */
    if (tracing) {

      if (breakpoint == Pc) {
	MoveCursor(term_lines, 0);
	printf("Break at $%.4X\n", Pc);
	return;
      }
      
      if (phantom_location == (long) Pc)
	phantom_location = -1;		/* start tracing now */
      
      if (phantom_location == -1
	  &&  trace_lo != -1
	  &&  (long) Pc >= trace_lo
	  &&  (long) Pc <= trace_hi) {
	diss(Pc, logging_fp);
	show_flags = TRUE;
      } else
	show_flags = FALSE;
    }
    
    
    do {					/* while (running) */


      /* MJHOSTET HACK */
#if 1
      if (!(mjhostet++ & 32767))
	if (PRIMARY_PAGE == screen_page)
	    redraw_hgr_screen (mem + 0x2000);
	else redraw_hgr_screen (mem + 0x4000);
#endif

#ifdef	JOYSTICK_SUPPORT
    if (joystick_counter_x > 0) --joystick_counter_x;
    if (joystick_counter_y > 0) --joystick_counter_y;
#endif	JOYSTICK_SUPPORT
      switch (mem[Pc++]) {
      case 0x00:			/* BRK */
	PCINC;
	push(high(Pc));
	push(low(Pc));
	B = 1;				/* set I flag? */
	push(get_status());
	Pc = join(mem[0xFFFE], mem[0xFFFF]);
	break;
	
      case 0x01:	/* ORA (indirect, X) */
	INDX;
	A |= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x02:
	break;
	
      case 0x03:
	break;
	
      case 0x04:	/* TSB zero page	65C02 */
	break;
	
      case 0x05:	/* ORA zero page */
	A |= rZEROP;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x06:	/* ASL zero page */
	ZEROP;
	val = REFzero << 1;
	CARRY(val);
	val &= 0xFF;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x07:
	break;
	
      case 0x08:	/* PHP */
	push(get_status());
	break;
	
      case 0x09:	/* ORA immediate */
	A |= REFimm;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x0A:	/* ASL accumulator */
	A = A << 1;
	CARRY(A);
	A &= 0xFF;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x0B:
	break;
	
      case 0x0C:	/* TSB absolute		65C02 */
	break;
	
      case 0x0D:	/* ORA absolute */
	ABSOL;
	A |= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x0E:	/* ASL absolute */
	ABSOL;
	val = REFptr << 1;
	CARRY(val);
	val &= 0xFF;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x0F:
	break;
	
      case 0x10:	/* BPL */
	if (N) {
	  PCINC;
	} else {
	  BRANCH();
	}
	break;
	
      case 0x11:	/* ORA (indirect), Y */
	INDY;
	A |= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x12:	/* ORA (indz)		65C02 */
	break;
	
      case 0x13:
	break;
	
      case 0x14:	/* TRB zero page	65C02 */
	break;
	
      case 0x15:	/* ORA zero page, X */
	A |= rZEROX;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x16:	/* ASL zero page, X */
	ZEROX;
	val = REFzero << 1;
	CARRY(val);
	val &= 0xFF;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x17:
	break;
	
      case 0x18:	/* CLC */
	C = 0;
	break;
	
      case 0x19:	/* ORA absolute, Y */
	ABSY;
	A |= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x1A:	/* INA			65C02 */
	break;
	
      case 0x1C:	/* TRB absolute		65C02 */
	break;
	
      case 0x1D:	/* ORA absolute, X */
	ABSX;
	A |= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x1E:	/* ASL absolute, X */
	ABSX;
	val = REFptr << 1;
	CARRY(val);
	val &= 0xFF;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x1F:
	break;
	
      case 0x20:	/* JSR */
	ptmp = REFimm;
	ptmp |= mem[Pc] << 8;
	push(high(Pc));
	push(low(Pc));
	Pc = ptmp;
	
	if (jmp_tbl[Pc])
	  jump(jmp_tbl[Pc]);
	break;
	
      case 0x21:	/* AND (indirect, X) */
	INDX;
	A &= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x22:
	break;
	
      case 0x23:
	break;
	
      case 0x24:	/* BIT zero page */
	tmp = rZEROP;
	ZERO(A & tmp);
	N = tmp & 0x80;
	V = tmp & 0x40;
	break;
	
      case 0x25:	/* AND zero page */
	A &= rZEROP;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x26:	/* ROL zero page */
	ZEROP;
	val = REFzero;
	tmp = C;
	val = val << 1;
	CARRY(val);
	val = (val & 0xFF) | tmp;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x27:
	break;
	
      case 0x28:	/* PLP */
	put_status(pop());
	break;
	
      case 0x29:	/* AND immediate */
	A &= REFimm;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x2A:	/* ROL accumulator */
	tmp = C;
	A = A << 1;
	CARRY(A);
	A = (A & 0xFF) | tmp;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x2B:
	break;
	
      case 0x2C:	/* BIT absolute */
	ABSOL;
	tmp = REFptr;
	ZERO(A & tmp);
	N = tmp & 0x80;
	V = tmp & 0x40;
	break;
	
      case 0x2D:	/* AND absolute */
	ABSOL;
	A &= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x2E:	/* ROL absolute */
	ABSOL;
	val = REFptr;
	tmp = C;
	val = val << 1;
	CARRY(val);
	val = (val & 0xFF) | tmp;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x2F:
	break;
	
      case 0x30:	/* BMI */
	if (N) {
	  BRANCH();
	} else{
	  PCINC;
	}
	break;
	
      case 0x31:	/* AND (indirect), Y */
	INDY;
	A &= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x32:	/* AND (indz)		65C02 */
	break;
	
      case 0x33:
	break;
	
      case 0x34:	/* BIT zero page, X	65C02 */
	break;
	
      case 0x35:	/* AND zero page, X */
	A &= rZEROX;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x36:	/* ROL zero page, X */
	ZEROX;
	val = REFzero;
	tmp = C;
	val = val << 1;
	CARRY(val);
	val = (val & 0xFF) | tmp;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x37:
	break;
	
      case 0x38:	/* SEC */
	C = 1;
	break;
	
      case 0x39:	/* AND absolute, Y */
	ABSY;
	A &= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x3A:	/* DEA			65C02 */
	break;
	
      case 0x3B:
	break;
	
      case 0x3C:	/* BIT absolute, X	65C02 */
	break;
	
      case 0x3D:	/* AND absolute, X */
	ABSX;
	A &= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x3E:	/* ROL absolute, X */
	ABSX;
	val = REFptr;
	tmp = C;
	val = val << 1;
	CARRY(val);
	val = (val & 0xFF) | tmp;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x3F:
	break;
	
      case 0x40:	/* RTI */
	put_status(pop());
	Pc = pop();
	Pc |= pop() << 8;
	break;
	
      case 0x41:	/* EOR (indirect, X) */
	INDX;
	A ^= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x42:
	break;
	
      case 0x43:
	break;
	
      case 0x44:
	break;
	
      case 0x45:	/* EOR zero page */
	A ^= rZEROP;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x46:	/* LSR zero page */
	ZEROP;
	tmp = REFzero;
	C = (tmp & 0x01);
	tmp = tmp >> 1;
	SETzero(tmp);
	N = 0;
	ZERO(tmp);
	break;
	
      case 0x47:
	break;
	
      case 0x48:	/* PHA */
	push(A);
	break;
	
      case 0x49:	/* EOR immediate */
	A ^= REFimm;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x4A:	/* LSR accumulator */
	C = (A & 0x01);
	A = A >> 1;
	N = 0;
	ZERO(A);
	break;
	
      case 0x4B:
	break;
	
      case 0x4C:	/* JMP absolute */
	ptmp = REFimm;
	ptmp |= REFimm << 8;
	Pc = ptmp;
	
	if (jmp_tbl[Pc])
	  jump(jmp_tbl[Pc]);
	break;
	
      case 0x4D:	/* EOR absolute */
	ABSOL;
	A ^= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x4E:	/* LSR absolute */
	ABSOL;
	tmp = REFptr;
	C = (tmp & 0x01);
	tmp = tmp >> 1;
	SETptr(tmp);
	N = 0;
	ZERO(tmp);
	break;
	
      case 0x4F:
	break;
	
      case 0x50:	/* BVC */
	if (V) {
	  PCINC;
	} else {
	  BRANCH();
	}
	break;
	
      case 0x51:	/* EOR (indirect), Y */
	INDY;
	A ^= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x52:	/* EOR (indz)		65C02 */
	break;
	
      case 0x53:
	break;
	
      case 0x54:
	break;
	
      case 0x55:	/* EOR zero page, X */
	A ^= rZEROX;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x56:	/* LSR zero page, X */
	ZEROX;
	tmp = REFzero;
	C = (tmp & 0x01);
	tmp = tmp >> 1;
	SETzero(tmp);
	N = 0;
	ZERO(tmp);
	break;
	
      case 0x57:
	break;
	
      case 0x58:	/* CLI */
	I = 0;
	break;
	
      case 0x59:	/* EOR absolute, Y */
	ABSY;
	A ^= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x5A:	/* PHY		65C02 */
	break;
	
      case 0x5B:
	break;
	
      case 0x5C:
	break;
	
      case 0x5D:	/* EOR absolute, X */
	ABSX;
	A ^= REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x5E:	/* LSR absolute, X */
	ABSX;
	tmp = REFptr;
	C = (tmp & 0x01);
	tmp = tmp >> 1;
	SETptr(tmp);
	N = 0;
	ZERO(tmp);
	break;
	
      case 0x5F:
	break;
	
      case 0x60:	/* RTS */
	DO_RTS;
	break;
	
      case 0x61:	/* ADC (indirect, X) */
	INDX;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x62:
	break;
	
      case 0x63:
	break;
	
      case 0x64:	/* STZ zero page	65C02 */
	break;
	
      case 0x65:	/* ADC zero page */
	val = rZEROP;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
	
      case 0x66:	/* ROR zero page */
	ZEROP;
	val = REFzero;
	tmp = C;
	C = val & 0x01;
	val = val >> 1;
	val |= tmp << 7;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x67:
	break;
	
      case 0x68:	/* PLA */
	A = pop();
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x69:	/* ADC immediate */
	val = REFimm;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x6A:	/* ROR accumulator */
	tmp = C;
	C = A & 0x01;
	A = A >> 1;
	A |= tmp << 7;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x6B:
	break;
	
      case 0x6C:	/* JMP indirect */
	ptmp = REFimm;
	ptmp |= REFimm << 8;
	Pc = mem[ptmp++];
	Pc |= mem[ptmp] << 8;
	
	if (jmp_tbl[Pc])
	  jump(jmp_tbl[Pc]);
	break;
	
      case 0x6D:	/* ADC absolute */
	ABSOL;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x6E:	/* ROR absolute */
	ABSOL;
	val = REFptr;
	tmp = C;
	C = val & 0x01;
	val = val >> 1;
	val |= tmp << 7;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x6F:
	break;
	
      case 0x70:	/* BVS */
	if (V) {
	  BRANCH();
	} else {
	  PCINC;
	}
	break;
	
      case 0x71:	/* ADC (indirect),Y */
	INDY;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x72:	/* ADC (indz)		65C02 */
	break;
	
      case 0x73:
	break;
	
      case 0x74:	/* STZ zero page, X	65C02 */
	break;
	
      case 0x75:	/* ADC zero page, X */
	val = rZEROX;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x76:	/* ROR zero page, X */
	ZEROX;
	val = REFzero;
	tmp = C;
	C = val & 0x01;
	val = val >> 1;
	val |= tmp << 7;
	SETzero(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x77:
	break;
	
      case 0x78:	/* SEI */
	I = 1;
	break;
	
      case 0x79:	/* ADC absolute, Y */
	ABSY;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x7A:	/* PLY		65C02 */
	break;
	
      case 0x7B:
	break;
	
      case 0x7C:	/* JMP (abs indirect, X)	65C02 */
	break;
	
      case 0x7D:	/* ADC absolute, X */
	ABSX;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  val += num + C;
	  dCARRY(val);
	  while (val >= 100)
	    val -= 100;
	  
	  A = tobcd(val);
	} else {
	  A += val + C;
	  CARRY(A);
	  A &= 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp == stmp) && (tmp != N);
	break;
	
      case 0x7E:	/* ROR absolute, X */
	ABSX;
	val = REFptr;
	tmp = C;
	C = val & 0x01;
	val = val >> 1;
	val |= tmp << 7;
	SETptr(val);
	SIGN(val);
	ZERO(val);
	break;
	
      case 0x7F:
	break;
	
      case 0x80:	/* BRA			65C02 */
	break;
	
      case 0x81:	/* STA (indirect, X) */
	INDX;
	SETptr(A);
	break;
	
      case 0x82:
	break;
	
      case 0x83:
	break;
	
      case 0x84:	/* STY zero page */
	sZEROP(Y);
	break;
	
      case 0x85:	/* STA zero page */
	sZEROP(A);
	break;
	
      case 0x86:	/* STX zero page */
	sZEROP(X);
	break;
	
      case 0x87:
	break;
	
      case 0x88:	/* DEY */
	Y--;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0x89:
	break;
	
      case 0x8A:	/* TXA */
	A = X;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x8B:
	break;
	
      case 0x8C:	/* STY absolute */
	ABSOL;
	SETptr(Y);
	break;
	
      case 0x8D:	/* STA absolute */
	ABSOL;
	SETptr(A);
	break;
	
      case 0x8E:	/* STX absolute */
	ABSOL;
	SETptr(X);
	break;
	
      case 0x8F:
	break;
	
      case 0x90:	/* BCC */
	if (C) {
	  PCINC;
	} else {
	  BRANCH();
	}
	break;
	
      case 0x91:	/* STA (indirect), Y */
	INDY;
	SETptr(A);
	break;
	
      case 0x92:
	break;
	
      case 0x93:
	break;
	
      case 0x94:	/* STY zero page, X */
	sZEROX(Y);
	break;
	
      case 0x95:	/* STA zero page, X */
	sZEROX(A);
	break;
	
      case 0x96:	/* STX zero page, Y */
	sZEROY(X);
	break;
	
      case 0x97:
	break;
	
      case 0x98:	/* TYA */
	A = Y;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0x99:	/* STA absolute, Y */
	ABSY;
	SETptr(A);
	break;
	
      case 0x9A:	/* TXS */
	Sp = X;
	break;
	
      case 0x9B:
	break;
	
      case 0x9C:	/* STZ absolute		65C02 */
	break;
	
      case 0x9D:	/* STA absolute, X */
	ABSX;
	SETptr(A);
	break;
	
      case 0x9E:	/* STZ absolute, X	65C02 */
	break;
	
      case 0x9F:
	break;
	
      case 0xA0:	/* LDY immediate */
	Y = REFimm;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xA1:	/* LDA (indirect, X) */
	INDX;
	A = REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xA2:	/* LDX immediate */
	X = REFimm;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xA3:
	break;
	
      case 0xA4:	 /* LDY zero page */
	Y = rZEROP;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xA5:	/* LDA zero page */
	A = rZEROP;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xA6:	/* LDX zero page */
	X = rZEROP;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xA7:
	break;
	
      case 0xA8:	/* TAY */
	Y = A;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xA9:	/* LDA immediate */
	A = REFimm;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xAA:	/* TAX */
	X = A;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xAB:
	break;
	
      case 0xAC:	/* LDY absolute */
	ABSOL;
	Y = REFptr;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xAD:	/* LDA absolute */
	ABSOL;
	A = REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xAE:	/* LDX absolute */
	ABSOL;
	X = REFptr;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xAF:
	break;
	
      case 0xB0:	/* BCS */
	if (C) {
	  BRANCH();
	} else {
	  PCINC;
	}
	break;
	
      case 0xB1:	/* LDA (indirect), Y */
	INDY;
	A = REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xB2:	/* LDA (indz)		65C02 */
	break;
	
      case 0xB3:
	break;
	
      case 0xB4:	/* LDY zero page, X */
	Y = rZEROX;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xB5:	/* LDA zero page, X */
	A = rZEROX;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xB6:	/* LDX zero page, Y */
	X = rZEROY;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xB7:
	break;
	
      case 0xB8:	/* CLV */
	V = 0;
	break;
	
      case 0xB9:	/* LDA absolute, Y */
	ABSY;
	A = REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xBA:	/* TSX */
	X = Sp;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xBB:
	break;
	
      case 0xBC:	/* LDY absolute, X */
	ABSX;
	Y = REFptr;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xBD:	/* LDA absolute, X */
	ABSX;
	A = REFptr;
	SIGN(A);
	ZERO(A);
	break;
	
      case 0xBE:	/* LDX absolute, Y */
	ABSY;
	X = REFptr;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xBF:
	break;
	
      case 0xC0:	/* CPY immediate */
	tmp = REFimm;
	C = (Y >= tmp);
	tmp = (Y - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xC1:	/* CMP (indirect, X) */
	INDX;
	tmp = REFptr;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xC2:
	break;
	
      case 0xC3:
	break;
	
      case 0xC4:	/* CPY zero page */
	tmp = rZEROP;
	C = (Y >= tmp);
	tmp = (Y - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xC5:	/* CMP zero page */
	tmp = rZEROP;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xC6:	/* DEC zero page */
	ZEROP;
	stmp = REFzero - 1;
	SETzero(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xC7:
	break;
	
      case 0xC8:	/* INY */
	Y++;
	SIGN(Y);
	ZERO(Y);
	break;
	
      case 0xC9:	/* CMP immediate */
	tmp = REFimm;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xCA:	/* DEX */
	X--;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xCB:
	break;
	
      case 0xCC:	/* CPY absolute */
	ABSOL;
	tmp = REFptr;
	C = (Y >= tmp);
	tmp = (Y - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xCD:	/* CMP absolute */
	ABSOL;
	tmp = REFptr;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xCE:	/* DEC absolute */
	ABSOL;
	stmp = REFptr - 1;
	SETptr(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xCF:
	break;
	
      case 0xD0:	/* BNE */
	if (NZ) {
	  BRANCH();
	} else {
	  PCINC;
	}
	break;
	
      case 0xD1:	/* CMP (indirect), Y */
	INDY;
	tmp = REFptr;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xD2:	/* CMP (indz)		65C02 */
	break;
	
      case 0xD3:
	break;
	
      case 0xD4:
	break;
	
      case 0xD5:	/* CMP zero page, X */
	tmp = rZEROX;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xD6:	/* DEC zero page, X */
	ZEROX;
	stmp = REFzero - 1;
	SETzero(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xD7:
	break;
	
      case 0xD8:	/* CLD */
	D = 0;
	break;
	
      case 0xD9:	/* CMP absolute, Y */
	ABSY;
	tmp = REFptr;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xDA:	/* PHX		65C02 */
	break;
	
      case 0xDB:
	break;
	
      case 0xDC:
	break;
	
      case 0xDD:	/* CMP absolute, X */
	ABSX;
	tmp = REFptr;
	C = (A >= tmp);
	tmp = (A - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xDE:	/* DEC absolute, X */
	ABSX;
	stmp = REFptr - 1;
	SETptr(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xDF:
	break;
	
      case 0xE0:	/* CPX immediate */
	tmp = REFimm;
	C = (X >= tmp);
	tmp = (X - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xE1:	/* SBC (indirect, X) */
	INDX;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xE2:
	break;
	
      case 0xE3:
	break;
	
      case 0xE4:	/* CPX zero page */
	tmp = rZEROP;
	C = (X >= tmp);
	tmp = (X - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xE5:	/* SBC zero page */
	val = rZEROP;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xE6:	/* INC zero page */
	ZEROP;
	stmp = REFzero + 1;
	SIGN(stmp);
	ZERO(stmp);
	SETzero(stmp);
	break;
	
      case 0xE7:
	break;
	
      case 0xE8:	/* INX */
	X++;
	SIGN(X);
	ZERO(X);
	break;
	
      case 0xE9:	/* SBC immediate */
	val = REFimm;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xEA:	/* NOP */
	break;
	
      case 0xEB:
	break;
	
      case 0xEC:	/* CPX absolute */
	ABSOL;
	tmp = REFptr;
	C = (X >= tmp);
	tmp = (X - tmp) & 0xFF;
	SIGN(tmp);
	ZERO(tmp);
	break;
	
      case 0xED:	/* SBC absolute */
	ABSOL;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xEE:	/* INC absolute */
	ABSOL;
	stmp = REFptr + 1;
	SETptr(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xEF:
	break;
	
      case 0xF0:	/* BEQ */
	if (NZ) {
	  PCINC;
	} else {
	  BRANCH();
	}
	break;
	
      case 0xF1:	/* SBC (indirect), Y */
	INDY;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xF2:	/* SBC (indz)		65C02 */
	break;
	
      case 0xF3:
	break;
	
      case 0xF4:
	break;
	
      case 0xF5:	/* SBC zero page, X */
	val = rZEROX;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xF6:	/* INC zero page, X */
	ZEROX;
	stmp = REFzero + 1;
	SETzero(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xF7:
	break;
	
      case 0xF8:	/* SED */
	D = 1;
	break;
	
      case 0xF9:	/* SBC absolute, Y */
	ABSY;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xFA:	/* PLX		65C02 */
	break;
	
      case 0xFB:
	break;
	
      case 0xFC:
	break;
	
      case 0xFD:	/* SBC absolute, X */
	ABSX;
	val = REFptr;
	tmp = A & 0x80;
	stmp = val & 0x80;
	if (D) {
	  /* --mlk */
	  num = tobinary(val);
	  val = tobinary(A);
	  
	  
	  foo = val - (num + !C);
	  C = (foo >= 0);
	  while (foo < 0)
	    foo += 100;

	  A = tobcd(foo);
	} else {
	  foo = A - (val + !C);
	  C = (foo >= 0);
	  A = foo & 0xFF;
	}
	ZERO(A);
	SIGN(A);
	V = (tmp != stmp) && (tmp != N);
	break;
	
      case 0xFE:	/* INC absolute, X */
	ABSX;
	stmp = REFptr + 1;
	SETptr(stmp);
	SIGN(stmp);
	ZERO(stmp);
	break;
	
      case 0xFF:
	break;
      }
    } while (running);
    
    if (show_flags)
      flags(logging_fp);
  } while (tracing);
}
