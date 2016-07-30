#import "WozView.h"
#import "hgrconv.h"

id screen_wozview;
unsigned char *wozview_buf;


void
redraw_hgr_screen (unsigned char *a2buf)
{
  static unsigned char a2_shadow[0x2000];

  [screen_wozview drawChangedAreas:a2buf shadow:a2_shadow];
  DPSFlush();
}
