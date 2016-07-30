#include <stdio.h>
#include <string.h>

static unsigned short one_to_two (int n);
static unsigned short color_one_to_two (int n, int odd);
static void printbin (unsigned short n);

int
main()
{
#if 0
  printbin (color_one_to_two (0x1, 1));
  printbin (color_one_to_two (0x1400, 1));


#else
  int i, j, k;
  static const unsigned short base[] = {
    0x2000, 0x2080, 0x2100, 0x2180, 0x2200, 0x2280, 0x2300, 0x2380,
    0x2028, 0x20A8, 0x2128, 0x21A8, 0x2228, 0x22A8, 0x2328, 0x23A8,
    0x2050, 0x20D0, 0x2150, 0x21D0, 0x2250, 0x22D0, 0x2350, 0x23D0 };
  unsigned char row[0x2000];

  memset (row, 0, 0x2000);
  for (i = 0; i < 24; i++)
    for (j = 0; j < 8; j++)
      for (k = 0; k < 40; k++)
	row[base[i] - 0x2000 + 0x400 * j + k] = i * 8 + j;


  /* Output addr_to_row. */

  puts ("/* addr_to_row[(addr & 0x1FFF) / 8] contains  */\n"
	"/* the screen row for a given hires address. */\n");

  fputs ("const unsigned char addr_to_row[] = {\n   ", stdout);
  for (i = 0; i < 0x2000; i += 8)
    {
      if (i && !(i % 64))
	printf ("       /* 0x%04X - 0x%04X */\n   ",
		i + 0x2000 - 64, i + 0x2000 - 1);
      printf ("%3d%s ", row[i], i == 0x2000 -8 ? "" : ",");
    }
  printf ("        /* 0x3FC0 - 0x3FFF */\n};\n", i + 0x2000 - 64);


  /* Output row_to_addr. */

  puts ("\n\n"
	"/* row_to_addr[row] contains the base address for a given row. */\n");
  fputs ("const unsigned int row_to_addr[] = {", stdout);
  for (i = 0; i < 24; i++)
    {
      fputs ("\n   ", stdout);
      for (j = 0; j < 8; j++)
	printf ("0x%04X%s ", base[i] + 0x400 * j - 0x2000,
		(i == 23 && j == 7) ? "" : ",");
    }
  puts ("\n};");

  puts ("\n\n"
	"#if !USE_GREYSCALE\n"
	"/* one_to_two_bpp[byte] maps one bit per pixel to two for the */\n"
	"/* low 7 bits, reversing the order.                           */\n");
  fputs ("const unsigned long one_to_two_bpp[] = {", stdout);
  
  for (i = 0; i < 32; i++)
    {
      fputs ("\n   ", stdout);
      for (j = 0; j < 8; j++)
	printf ("0x%04X%s ",
		(unsigned long) (one_to_two (i * 8 + j)),
		(i == 31 && j == 7) ? "" : ",");
    }
  puts ("\n};");

  puts ("\n#else\n"
	"/* color_one_to_two_bpp[13 bits] is like one_to_two_bpp, except */\n"
	"/* that it maps a set of pixels to a 2-bit color version of     */\n"
	"/* those pixels.  The 13 pixels are the five to the left of a   */\n"
	"/* given screen byte and the screen byte itself.  The output    */\n"
	"/* represents the rightmost two pixels of the byte to the left  */\n"
	"/* and the leftmost 5 pixels of the current byte.               */\n"
	"/* Note that not all pixels are converted; two on each edge are */\n"
	"/* only there to  convey information about how to color the     */\n"
	"/* interior pixels.  The color bit of the byte is ignored.      */\n"
	);

  fputs ("const unsigned short color_one_to_two_bpp_even[] = {",stdout);
  for (i = 0; i < 16384 / 8; i++)
    {
      fputs ("\n   ", stdout);
      for (j = 0; j < 8; j++)
	printf ("0x%04X%s ",
		(unsigned long) (color_one_to_two (i * 8 + j, 0)),
		(i == 16384 / 8 - 1 && j == 7) ? "" : ",");
    }
  puts ("\n};\n");

  fputs ("const unsigned short color_one_to_two_bpp_odd[] = {", stdout);
  for (i = 0; i < 16384 / 8; i++)
    {
      fputs ("\n   ", stdout);
      for (j = 0; j < 8; j++)
	printf ("0x%04X%s ",
		(unsigned long) (color_one_to_two (i * 8 + j, 1)),
		(i == 16384 / 8 - 1 && j == 7) ? "" : ",");
    }
  puts ("\n};\n#endif");

#endif

  return 0;
}


static unsigned short
one_to_two (int n)
{
  unsigned short res = 0;
  int mask;

  for (mask = 1; mask != 128; res <<= 2, mask <<= 1)
    if (n & mask)
      res |= 3;
  return res >> 2;
}

static unsigned short
color_one_to_two (int n, int odd)
{
  int bitnum;
  int i, new;
  static const int bitmask[] = {
    1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1, 2, 4, 8, 16, 32, 64
    };
  unsigned short res;

/* For each bit:
   1) All pixels start out black.
   For each bit:
   2) If bit is on, see if either neighbor is on.
     a) If either is on, make this bit white.
     b) If neither is on, see if any neighbors 2-away are on.  If either is on,
       i) If our bitnumber + odd is an odd number, we are light gray.
       ii) Else, we are dark gray.
     c) If we are gray, make both neighbors be our color.  
   3) If this bit is off, go on to the next one.
*/

  /* Convert it to "canonical form" here (punt unused bit, reorder so
   * leftmost pixels are high bits, etc.)
   */

  new = 0;
  for (i = 0; i < 13; i++)
    if (n & bitmask[i])
      new |= (1 << (12 - i));
  n = new;

/*
  printf ("Converting: "); printbin (n);
*/

  /* At this point we have 13 bits; 3 on the left and 3 on the right for
   * information, and 7 to generate pixels for.
   */

  res = 0;  /* Default to black. */
#define BIT(b) ((n & (1 << ((b) + 2))) != 0)

  /* Convert 9 bits instead of 7 so color outside the bits in question
   * can bleed back in.
   */


  for (bitnum = 0; bitnum < 9; bitnum++)
    {
      if (BIT (bitnum))
	{
	  if (BIT (bitnum + 1) || BIT (bitnum - 1)
	      || !(BIT (bitnum + 2) || BIT (bitnum - 2)))
	    {
	      res |= 3 << (bitnum * 2);
	    }
	  else
	    {
	      /* Go ahead, just TRY to figure this out.  :-) */
	      res |= (((1L | 4 | 16) << ((bitnum * 2) + !((bitnum + odd) & 1)))
		      >> 2);
	    }
	}
    }

  return ((res >> 2) & 0x3FFF);   /* Only 14 bits. */
}


static void
printbin (unsigned short n)
{
  unsigned short mask;

  for (mask = 0x8000; mask != 0; mask >>= 1)
    putchar ('0' + !!(n & mask));
  putchar ('\n');
}
