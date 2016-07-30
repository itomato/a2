/*
 *     hgrconv.h
 */

extern void apple_to_next (const unsigned char *a2buf,
#ifndef	USE_COLOR
			   unsigned char nextbuf[192][72],
#else	USE_COLOR
			   unsigned char nextbuf[192][280],
#endif	USE_COLOR
			   int first_row, int height);
extern int find_changed_areas (const unsigned char *scr1,
			       const unsigned char *scr2,
			       unsigned char changed[192/16]);
extern void copy_changed_areas (const unsigned char *src, unsigned char *dst,
				const unsigned char changed[192/16]);
