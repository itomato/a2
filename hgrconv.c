#ifndef USE_GREYSCALE
#define USE_GREYSCALE 1
#endif

#include <c.h>
#include <string.h>
#include "hgrtable.h"
#include "hgrconv.h"

/* for byte swapping -- NXSwapBigLongToHost */
#include <sys/types.h>
#include <netinet/in.h>

#if ((!USE_GREYSCALE) || (!USE_COLOR)
void
apple_to_next (const unsigned char *a2buf, unsigned char nextbuf[192][72],
	       int first_row, int height)
{
  const unsigned int *row_base = row_to_addr + first_row;
  unsigned long *nbuf = (unsigned long *) &nextbuf[first_row][0];
  const unsigned char *p;
  short rows_left, n;

  for (rows_left = height - 1; rows_left != -1; rows_left--)
    {
      p = a2buf + *row_base++;
      for (n = 4; n != -1; n--)
	{
#if 0    /* Coerce compiler into using bitfield instructions.  Nonportable. */
	  /* Oddly enough, the shift-and-mask code below is faster, even
	   * though it's ~twice as many instructions.  The bitfield code
           * performs 8 read-modify-writes as opposed to 4 writes.  Must be
           * the difference.
	   * I'll leave this here cuz it looks cool...
	   */
	  void *buf = (void *) nbuf;

	  ((struct { int a:14, b:14; } *) buf)->a      = one_to_two_bpp[*p++];
	  ((struct { int a:14, b:14; } *) buf)->b      = one_to_two_bpp[*p++];
	  ((struct { int a:12, b:14; } *) (buf+2))->b  = one_to_two_bpp[*p++];
	  ((struct { int a:10, b:14; } *) (buf+4))->b  = one_to_two_bpp[*p++];
	  ((struct { int a:8,  b:14; } *) (buf+6))->b  = one_to_two_bpp[*p++];
	  ((struct { int a:6,  b:14; } *) (buf+8))->b  = one_to_two_bpp[*p++];
	  ((struct { int a:4,  b:14; } *) (buf+10))->b = one_to_two_bpp[*p++];
	  ((struct { int a:18, b:14; } *) (buf+10))->b = one_to_two_bpp[*p++];
	  nbuf = (void *) nbuf + 14;
	  
#else
	  unsigned long a, b, c;

	  /* NOTE: This assumes a big-endian CPU! */
	  a = one_to_two_bpp[*p++] << 18;
	  b = one_to_two_bpp[*p++] << 4;
	  c = one_to_two_bpp[*p++];
	  *nbuf++ = a | b | (c >> 10);
	  a = one_to_two_bpp[*p++] << 8;
	  b = one_to_two_bpp[*p++];
	  *nbuf++ = (c << 22) | a | (b >> 6);
	  a = one_to_two_bpp[*p++] << 12;
	  c = one_to_two_bpp[*p++];
	  *nbuf++ = (b << 26) | a | (c >> 2);
	  a = one_to_two_bpp[*p++];
	  *((unsigned short *) nbuf)++ = a | (c << 14);
#endif
	}
    }
}
#else

#ifndef	USE_COLOR

/* Define the Reg type. */
#ifdef	__LITTLE_ENDIAN__
/* Assumes big endian: */
typedef union {
  struct {
    unsigned char c;
    unsigned char hi;
    char filler2;
    char filler1;
  } c;
  struct {
    unsigned short s;
    unsigned short filler;
  } s;
  unsigned long l;
} Reg;
#else	__LITTLE_ENDIAN__
/* Assumes big endian: */
typedef union {
  struct {
    char filler1, filler2;
    unsigned char hi;
    unsigned char c;
  } c;
  struct {
    unsigned short filler;
    unsigned short s;
  } s;
  unsigned long l;
} Reg;
#endif	__LITTLE_ENDIAN__

void
apple_to_next (const unsigned char *a2buf,
	       unsigned char nextbuf[192][72],
	       int first_row, int height)
{
  const unsigned int *row_base = row_to_addr + first_row;
  unsigned long *nbuf = (unsigned long *) &nextbuf[first_row][0];
  const unsigned char *p;
  register short rows_left, n;

  /* NOTE: This assumes a big-endian CPU! */

  for (rows_left = height - 1; rows_left >= 0; rows_left--)
    {
      Reg a2bits;
      unsigned long a, b, c;

#define NEXT_A2BITS a2bits.s.s &= 0x7F; a2bits.s.s <<= 7; a2bits.c.c = *p++


      p = a2buf + *row_base++;

      /* Left edge - 5 bytes */

      /* Create the first long and ship it out. */
      a2bits.l = *p++;
      a = color_one_to_two_bpp_even[a2bits.l] << 24;
      NEXT_A2BITS;
      b = color_one_to_two_bpp_odd[a2bits.l] << 10;
      NEXT_A2BITS;
      c = color_one_to_two_bpp_even[a2bits.l];
      *nbuf++ = NXSwapBigLongToHost( a | b | (c >> 4));
      
      /* Create the second long and ship it out. */
      NEXT_A2BITS;
      a = color_one_to_two_bpp_odd[a2bits.l] << 14;
      NEXT_A2BITS;
      b = color_one_to_two_bpp_even[a2bits.l];
      *nbuf++ = NXSwapBigLongToHost((c << 28) | a | b);

      /* Middle - 32 bytes */
      for (n = 1; n >= 0; n--)
	{
	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_odd[a2bits.l] << 18;
	  NEXT_A2BITS;
	  b = color_one_to_two_bpp_even[a2bits.l] << 4;
	  NEXT_A2BITS;
	  c = color_one_to_two_bpp_odd[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost(a | b | (c >> 10));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_even[a2bits.l] << 8;
	  NEXT_A2BITS;
	  b = color_one_to_two_bpp_odd[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((c << 22) | a | (b >> 6));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_even[a2bits.l] << 12;
	  NEXT_A2BITS;
	  c = color_one_to_two_bpp_odd[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((b << 26) | a | (c >> 2));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_even[a2bits.l] << 16;
	  NEXT_A2BITS;
	  a |= color_one_to_two_bpp_odd[a2bits.l] << 2;
	  NEXT_A2BITS;
	  b = color_one_to_two_bpp_even[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((c << 30) | a | (b >> 12));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_odd[a2bits.l] << 6;
	  NEXT_A2BITS;
	  c = color_one_to_two_bpp_even[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((b << 20) | a | (c >> 8));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_odd[a2bits.l] << 10;
	  NEXT_A2BITS;
	  b = color_one_to_two_bpp_even[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((c << 24) | a | (b >> 4));

	  NEXT_A2BITS;
	  a = color_one_to_two_bpp_odd[a2bits.l] << 14;
	  NEXT_A2BITS;
	  c = color_one_to_two_bpp_even[a2bits.l];
	  *nbuf++ = NXSwapBigLongToHost((b << 28) | a | c);
	}

      /* Right edge - three bytes. */
      NEXT_A2BITS;
      a = color_one_to_two_bpp_odd[a2bits.l] << 18;
      NEXT_A2BITS;
      b = color_one_to_two_bpp_even[a2bits.l] << 4;
      NEXT_A2BITS;
      c = color_one_to_two_bpp_odd[a2bits.l];
      *nbuf++ = NXSwapBigLongToHost(a | b | (c >> 10));

      /* Handle trailing stuff. */
      a2bits.s.s &= 0x7F; a2bits.s.s <<= 7;
      *nbuf++ = NXSwapBigLongToHost((c << 22) | (color_one_to_two_bpp_even[a2bits.l] << 8));
    }
}

#else	USE_COLOR

#define COLOR_TYPE	unsigned char

#define A2_COLORBLACK	0x00	/* (00 red, 00 green, 00 blue, 00 alpha) */
#define A2_COLORGREEN	0x30	/* (00 red, 11 green, 00 blue, 00 alpha) */
#define A2_COLORVIOLET	0xcc	/* (11 red, 00 green, 11 blue, 00 alpha) */
#define A2_COLORWHITE	0xfc	/* (11 red, 11 green, 11 blue, 00 alpha) */
/* also	A2_COLORBLACK	4 */
#define A2_COLORORANGE	0xd0	/* (11 red, 01 green, 00 blue, 00 alpha) */
#define A2_COLORBLUE	0x0c	/* (00 red, 00 green, 11 blue, 00 alpha) */
/* also	A2_COLORWHITE	7 */

#define A2_COLORRED	0xc0	/* (11 red, 00 green, 00 blue, 00 alpha) */


#define BIT(b) ((bytes & (1 << (b))) != 0)

inline COLOR_TYPE
color_one_to_eight_bpp0_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(0 + 8))
    {
	if ((BIT(-2 + 8)) || (BIT(1 + 8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7 + 8))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(-2 + 8)) && (!BIT(-3 + 8))) || ((BIT(1 + 8)) && (!BIT(2 + 8))))
	{
	    if (BIT(7 + 8)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp1_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(1 + 8))
    {
	if ((BIT(0 + 8)) || (BIT(2 + 8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7 + 8))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(0 + 8)) && (!BIT(-2 + 8))) || ((BIT(2 + 8)) && (!BIT(3 + 8))))
	{
	    if (BIT(7 + 8)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp2_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(2))
    {
	if ((BIT(1)) || (BIT(3)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(1)) && (!BIT(0))) || ((BIT(3)) && (!BIT(4))))
	{
	    if (BIT(7)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp3_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(3))
    {
	if ((BIT(2)) || (BIT(4)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(2)) && (!BIT(1))) || ((BIT(4)) && (!BIT(5))))
	{
	    if (BIT(7)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp4_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(4))
    {
	if ((BIT(3)) || (BIT(5)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(3)) && (!BIT(2))) || ((BIT(5)) && (!BIT(6))))
	{
	    if (BIT(7)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp5_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(5))
    {
	if ((BIT(4)) || (BIT(6)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(4)) && (!BIT(3))) || ((BIT(6)) && (!BIT(8))))
	{
	    if (BIT(7)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp6_even(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(6))
    {
	if ((BIT(5)) || (BIT(8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORBLUE;
	    }
	    else color = A2_COLORVIOLET;
	}
    }
    else
    {
	if (((BIT(5)) && (!BIT(4))) || ((BIT(8)) && (!BIT(9))))
	{
	    if (BIT(7)) color = A2_COLORORANGE;
	    else color = A2_COLORGREEN;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp0_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(0 + 8))
    {
	if ((BIT(-2 + 8)) || (BIT(1 + 8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7 + 8))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(-2 + 8)) && (!BIT(-3 + 8))) || ((BIT(1 + 8)) && (!BIT(2 + 8))))
	{
	    if (BIT(7 + 8)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp1_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(1 + 8))
    {
	if ((BIT(0 + 8)) || (BIT(2 + 8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7 + 8))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(0 + 8)) && (!BIT(-2 + 8))) || ((BIT(2 + 8)) && (!BIT(3 + 8))))
	{
	    if (BIT(7 + 8)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp2_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(2))
    {
	if ((BIT(1)) || (BIT(3)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(1)) && (!BIT(0))) || ((BIT(3)) && (!BIT(4))))
	{
	    if (BIT(7)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp3_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(3))
    {
	if ((BIT(2)) || (BIT(4)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(2)) && (!BIT(1))) || ((BIT(4)) && (!BIT(5))))
	{
	    if (BIT(7)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp4_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(4))
    {
	if ((BIT(3)) || (BIT(5)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(3)) && (!BIT(2))) || ((BIT(5)) && (!BIT(6))))
	{
	    if (BIT(7)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp5_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(5))
    {
	if ((BIT(4)) || (BIT(6)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(4)) && (!BIT(3))) || ((BIT(6)) && (!BIT(8))))
	{
	    if (BIT(7)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

inline COLOR_TYPE
color_one_to_eight_bpp6_odd(unsigned short bytes)
{
	/* bytes is 4 bytes.  The first byte and third byte are just for
		information.  The fourth byte is padding.
		Data is in the second byte
	*/

    COLOR_TYPE color;

    if (BIT(6))
    {
	if ((BIT(5)) || (BIT(8)))
	{
	    color = A2_COLORWHITE;
	}
	else
	{
	    if (BIT(7))
	    {
		color = A2_COLORORANGE;
	    }
	    else color = A2_COLORGREEN;
	}
    }
    else
    {
	if (((BIT(5)) && (!BIT(4))) || ((BIT(8)) && (!BIT(9))))
	{
	    if (BIT(7)) color = A2_COLORBLUE;
	    else color = A2_COLORVIOLET;
	}
	else color = A2_COLORBLACK;
    }

    return color;
}

/* Define the Reg type. */
#ifdef	__LITTLE_ENDIAN__
/* Assumes little endian: */
    typedef union {
	struct {
	    unsigned char c;
	    unsigned char hi;
	} c;
	unsigned short s;
    } Reg;
#else	__LITTLE_ENDIAN__
/* Assumes big endian: */
    typedef union {
	struct {
	    unsigned char hi;
	    unsigned char c;
	} c;
	unsigned short s;
    } Reg;
#endif	__LITTLE_ENDIAN__

void
apple_to_next(const unsigned char *a2buf,
		/* the first 7 and last 1 bytes are offscreen */
	       unsigned char nextbuf[192][280],
	       int first_row, int height)
{
    const unsigned int *row_base = row_to_addr + first_row;
    unsigned char *nbuf = &nextbuf[first_row][0];
    const unsigned char *p;
    register short rows_left;

    /* NOTE: This assumes a little-endian CPU! */

    /* just a counter loop; really does from first_row to first_row + height */
    for (rows_left = height - 1; rows_left >= 0; rows_left--)
    {
	Reg a2bits;
	int columns;
	
/* put 7-bits on one end of a2bits, so that bits 17-23 have the current */
/* working byte */
#define NEXT_A2BITS a2bits.s >>= 8; a2bits.c.hi = *p++

	p = a2buf + *row_base++;

	/* Create the first long and ship it out. */
	a2bits.s = 0;
	a2bits.c.hi = *p++;

	/* padding for first 7 display bytes */
/*
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
	*nbuf++ = A2_COLORRED;
*/

	/* just a counter loop, but the columns value should be right */
	for (columns = 0; columns < 259; columns += 14)
	{	    
	    *nbuf++ = color_one_to_eight_bpp0_even(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp1_odd(a2bits.s);

	    /* shove the next 8 bits onto the bit stream */
	    NEXT_A2BITS;

	    *nbuf++ = color_one_to_eight_bpp2_even(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp3_odd(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp4_even(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp5_odd(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp6_even(a2bits.s);
	    

	    *nbuf++ = color_one_to_eight_bpp0_odd(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp1_even(a2bits.s);

	    /* shove the next 8 bits onto the bit stream */
	    NEXT_A2BITS;

	    *nbuf++ = color_one_to_eight_bpp2_odd(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp3_even(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp4_odd(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp5_even(a2bits.s);
	    *nbuf++ = color_one_to_eight_bpp6_odd(a2bits.s);
	}
	
	*nbuf++ = color_one_to_eight_bpp0_even(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp1_odd(a2bits.s);

	/* shove the next 8 bits onto the bit stream */
	NEXT_A2BITS;

	*nbuf++ = color_one_to_eight_bpp2_even(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp3_odd(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp4_even(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp5_odd(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp6_even(a2bits.s);
	

	*nbuf++ = color_one_to_eight_bpp0_odd(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp1_even(a2bits.s);

	/* shove the next 8 bits onto the bit stream */
	a2bits.s >>= 8; a2bits.c.hi = 0;

	*nbuf++ = color_one_to_eight_bpp2_odd(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp3_even(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp4_odd(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp5_even(a2bits.s);
	*nbuf++ = color_one_to_eight_bpp6_odd(a2bits.s);

	/* padding for last byte of screen map */
//	*nbuf++ = A2_COLORRED;
    }
}

#endif	USE_COLOR

#endif	((!USE_GREYSCALE) || (!USE_COLOR)

/* Compares 40 bytes, returning TRUE iff they are equal, FALSE otherwise. */
/* Requires p1, p2 refer to long-addressable addresses.                   */

static inline int
equal40bytes (const unsigned char *p1, const unsigned char *p2)
{
  const unsigned long *l1 = (const unsigned long *) p1;
  const unsigned long *l2 = (const unsigned long *) p2;

  return (   (*l1++ == *l2++) && (*l1++ == *l2++) && (*l1++ == *l2++)
	  && (*l1++ == *l2++) && (*l1++ == *l2++) && (*l1++ == *l2++)
	  && (*l1++ == *l2++) && (*l1++ == *l2++) && (*l1++ == *l2++)
	  && (*l1++ == *l2++));
}


/* Finds the areas of the screens that differ in scr1 and scr2.  It breaks the
 * screens into 12 16-row sections.  If any two bytes differ in any section,
 * changed[section number] is set to TRUE, otherwise FALSE.
 * Returns FALSE iff scr1 and scr2 are identical screens.
 */

int
find_changed_areas (const unsigned char *scr1, const unsigned char *scr2,
		    unsigned char changed[192/16])
{
  const unsigned int *rta, *rta_base;
  short base;
  short i;
  int diff = FALSE;
  unsigned char *ch;

  /* Zero all the changed flags. */
  /* FIXME - zero as longs?  Is that safe? */
  for (i = (192 / 16) - 1, ch = &changed[0]; i >= 0; i--)
    *ch++ = FALSE;

#if 0
  /* Try each 16 row section.  As soon as we hit a difference, go to next. */
  rta_base = row_to_addr + 192 - 16;
  for (base = 11; base >= 0; rta_base -= 16, base--)
    for (i = 15, rta = rta_base; i >= 0; rta++, i--)
      if (!equal40bytes (scr1 + *rta, scr2 + *rta))
        {
          changed[base] = diff = TRUE;
          break;
        }
#else
  /* Try each 16 row section.  As soon as we hit a difference, go to next. */
  rta_base = row_to_addr + 192 - 16;
  for (base = 11; base >= 0; rta_base -= 16, base--)
    for (i = 15, rta = rta_base; i >= 0; rta++, i--)
      if (!equal40bytes (scr1 + *rta, scr2 + *rta))
        {
          changed[base] = diff = TRUE;
          break;
        }
#endif

#if 0  /* Old code. */
  /* Try each 16 row section.  As soon as we hit a difference, go to next. */
  for (base = 192 - 16; base >= 0; base -= 16)
    for (i = 15, rta = row_to_addr + base; i != -1; rta++, i--)
      if (cmp40bytes (scr1 + *rta, scr2 + *rta) >= 0)
	{
	  changed[base / 16] = diff = TRUE;
	  break;
	}
#endif


  return diff;
}


static inline void
copy40bytes (const unsigned char *src, unsigned char *dst)
{
  const unsigned long *s = (const unsigned long *) src;
  unsigned long *d = (unsigned long *) dst;

  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d++ = *s++;
  *d   = *s;
}


void
copy_changed_areas (const unsigned char *src, unsigned char *dst,
		    const unsigned char changed[192/16])
{
  short i;
  long base;
  const unsigned int *rta;
  const unsigned char *ch = &changed[192/16];

  for (base = 192 - 16; base >= 0; base -= 16)
    if (*--ch)
      for (i = 15, rta = row_to_addr + base; i >= 0; rta++, i--)
#if 0
	copy40bytes (src + *rta, dst + *rta);
#else   /* gcc 2 does Good Things with memcpy. */
	memcpy ((long *) (dst + *rta), (long *) (src + *rta), 40);
#endif
}
