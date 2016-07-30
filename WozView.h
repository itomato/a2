
/* Generated by Interface Builder */

#import <appkit/View.h>

@interface WozView:View
{
#ifndef	USE_COLOR
  unsigned char fbuf[192][72];  /* Pointer to the frame buffer for screen. */
#else	USE_COLOR
  unsigned char fbuf[192][280];  /* Pointer to the frame buffer for screen. */
#endif	USE_COLOR
}

- appDidInit:(id) sender;

- initFrame:(const NXRect *) frameRect;
- drawSelf:(const NXRect *) rects: (int) rectCount;

- drawChangedAreas:(const unsigned char *) a2_screen
                   shadow:(unsigned char *) shadow;
- (unsigned char *) data;
@end
