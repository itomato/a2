/*
 *     hgrtable.h
 */

extern const unsigned char addr_to_row[];
extern const unsigned int row_to_addr[];
#if !USE_GREYSCALE
extern const unsigned long one_to_two_bpp[];
#else
extern const unsigned short color_one_to_two_bpp_even[];
extern const unsigned short color_one_to_two_bpp_odd[];
#endif
