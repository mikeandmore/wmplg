#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern Display* dpy;

#define ROOT DefaultRootWindow(dpy)
#define XA_UTF8 XInternAtom(dpy, "UTF8_STRING", False)

void*        get_propv(Window win, ...);

void*        get_prop(Window win, Atom prop_type, const char* prop_name,
                      size_t* size_ret);


#endif /* _INTERNAL_H_ */
