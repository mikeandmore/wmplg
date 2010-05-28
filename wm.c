
#include "wm.h"
#include "internal.h"

Display* dpy = NULL;

void
init()
{
    dpy = XOpenDisplay(NULL);
}

void
finalize()
{
    XCloseDisplay(dpy);
}

void
wm_frame_move(Window wmframe, int x, int y)
{
    XMoveWindow(dpy, wmframe, x, y);
    XFlush(dpy);
}

void
wm_frame_resize(Window wmframe, unsigned int w, unsigned int h)
{
    XResizeWindow(dpy, wmframe, w, h);
    XFlush(dpy);
}
