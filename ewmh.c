
#include "wm.h"
#include "internal.h"

#define MAX_PROPERTY_VALUE_LEN 1024*1024*8

static bool
send_msg(Window win, const char* msg, ...)
{
    va_list alist;
    int i = 0;
    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(dpy, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    memset(event.xclient.data.l, 0, sizeof(long) * 5);
    
    va_start(alist, msg);
    while (i > 4) {
        long data = va_arg(alist, long);
        if (data == -1l)
            break;
        event.xclient.data.l[i] = data;
        i++;
    }
    va_end(alist);

    if (XSendEvent(dpy, DefaultRootWindow(dpy), False,
                   SubstructureRedirectMask | SubstructureNotifyMask,
                   &event)) {
        return true;
    } else {
        return false;
    }
    XFlush(dpy);
}

void*
get_prop(Window win, Atom prop_type, const char* prop_name,
         size_t* size_ret)
{
    Atom prop_atom;
    Atom type;
    int format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char* props;

    prop_atom = XInternAtom(dpy, prop_name, False);
    if (XGetWindowProperty(dpy, win, prop_atom, 0,
                           MAX_PROPERTY_VALUE_LEN / 4,
                           False, prop_type, &type, &format, &nitems,
                           &bytes_after, &props) != Success) {
        if (size_ret) *size_ret = 0;
        return NULL;
    }

    if (type != prop_type) {
        if (size_ret) *size_ret = 0;
        return NULL;
    }
    size_t sz = (format / 8) * nitems;
    /* a hack on this. in X11, seems that format/8 only indicates the size
     * of a specific atom. While real data structure might be larger than
     * atom on 64 platform. So here we'll do a scale here.
     */
    sz *= sizeof(long) / 4;

    void* ret_data = malloc(sz + 1);
    memset(ret_data, 0, sz + 1);
    memcpy(ret_data, props, sz);
    XFree(props);
    if (size_ret)
        (*size_ret) = nitems;
    return ret_data;
}

void*
get_propv(Window win, ...)
{
    va_list alist;
    Atom prop_type = 0;
    const char* name = NULL;
    size_t* ret_size = NULL;
    void* ret = NULL;
    
    va_start(alist, win);
    while (true) {
        if ((prop_type = va_arg(alist, Atom)) == 0)
            break;
        name = va_arg(alist, const char*);
        ret_size = va_arg(alist, size_t*);
        ret = get_prop(win, prop_type, name, ret_size);
        if (ret != NULL)
            break;
    }
    va_end(alist);
    return ret;
}

bool ewmh_supports(const char* prop)
{
    Atom xa_prop = XInternAtom(dpy, prop, False);
    int i;
    size_t size;
    Atom* list = get_prop(ROOT, XA_ATOM, "_NET_SUPPORTED", &size);
    
    if (list == NULL)
        return false;
    for (i = 0; i < size / sizeof(Atom); i++) {
        if (list[i] == xa_prop) {
            free(list);
            return true;
        }
    }
    free(list);
    return false;
}

bool
ewmh_wm_info(wminfo_t* info)
{
    if (info == NULL)
        return false;

    Window* sup_window = NULL;
    char* wm_name = NULL;
    char* wm_class = NULL;
    memset(info, 0, sizeof(wminfo_t));

    sup_window = get_propv(ROOT,
                           XA_WINDOW, "_NET_SUPPORTING_WM_CHECK", NULL,
                           XA_CARDINAL, "_WIN_SUPPORTING_WM_CHECK", NULL,
                           0);
    if (!sup_window)
        return false;
    wm_name = get_propv(*sup_window,
                        XA_UTF8, "_NET_WM_NAME", NULL,
                        XA_STRING, "_NET_WM_NAME", NULL,
                        0);
    wm_class = get_propv(*sup_window,
                         XA_UTF8, "WM_CLASS", NULL,
                         XA_STRING, "WM_CLASS", NULL,
                         0);
    unsigned long* pid = get_prop(*sup_window, XA_CARDINAL, "_NET_WM_PID",
                                  NULL);
    if (pid != NULL) {
        info->wm_pid = *pid;
        free(pid);
    }

    unsigned long* show_desktop = get_prop(ROOT, XA_CARDINAL,
                                           "_NET_SHOWING_DESKTOP", NULL);
    if (show_desktop != NULL) {
        info->wm_showing_desktop = *show_desktop;
        free(show_desktop);
    }

    if (wm_name) {
        strncpy(info->wm_name, wm_name, 256);
        free(wm_name);
    }
    if (wm_class) {
        strncpy(info->wm_class, wm_class, 256);
        free(wm_class);
    }
    return true;
}

Window*
ewmh_list_clients(size_t* sz_ret)
{
    Window* client_list = get_propv(ROOT,
                                    XA_WINDOW, "_NET_CLIENT_LIST", sz_ret,
                                    XA_CARDINAL, "_WIN_CLIENT_LIST", sz_ret,
                                    0);
    return client_list;
}

desktop_id_t
ewmh_get_desktop(Window client)
{
    desktop_id_t* desk = get_propv(client,
                                   XA_CARDINAL, "_NET_WM_DESKTOP", NULL,
                                   XA_CARDINAL, "_WIN_WORKSPACE", NULL,
                                   0);
    if (desk == NULL)
        return 0l;
    desktop_id_t ret = *desk;
    free(desk);
    return ret;
}

desktop_id_t
ewmh_get_current_desktop(void)
{
    desktop_id_t* desk = get_propv(ROOT,
                                   XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL,
                                   XA_CARDINAL, "_WIN_WORKSPACE", NULL,
                                   0);
    if (desk == NULL)
        return 0l;
    desktop_id_t ret = *desk;
    free(desk);
    return ret;
}

void
ewmh_set_current_desktop(desktop_id_t desk)
{
    if (!send_msg(ROOT, "_NET_CURRENT_DESKTOP", desk, -1)) {
        fprintf(stderr, "Can't switch current desktop\n");
    }
}

void
ewmh_set_active_window(Window client, bool switch_desktop)
{
    desktop_id_t win_desk = ewmh_get_desktop(client);
    desktop_id_t cur_desk = ewmh_get_current_desktop();
    if (win_desk != cur_desk) {
        if (switch_desktop)
            ewmh_set_current_desktop(win_desk);
        else
            return;
    }

    Window curwin = ewmh_get_active_window();
    if (client == curwin)
        return;
    
    if (!send_msg(client, "_NET_ACTIVE_WINDOW", 0, 0, curwin, -1)) {
        fprintf(stderr, "Can't set active window\n");
    }
    XMapRaised(dpy, client);
    XFlush(dpy);
}

Window
ewmh_get_active_window(void)
{
    Window* active_window = get_prop(ROOT, XA_WINDOW, "_NET_ACTIVE_WINDOW",
                                     NULL);
    if (active_window == NULL)
        return 0;
    Window ret = *active_window;
    free(active_window);
    return ret;
}

void
ewmh_get_window_info(Window client, window_info_t* info)
{
    if (info == NULL)
        return;
    
    char* wm_name = get_propv(client,
                              XA_UTF8, "_NET_WM_NAME", NULL,
                              XA_STRING, "WM_NAME", NULL,
                              0);
    char* wm_class = get_propv(client,
                               XA_UTF8, "WM_CLASS", NULL,
                               XA_STRING, "WM_CLASS", NULL,
                               0);
    size_t size;
    unsigned long* wm_icon_data = get_prop(client, XA_CARDINAL,
                                           "_NET_WM_ICON", &size);
    memset(info, 0, sizeof(window_info_t));
    if (wm_name) {
        strncpy(info->wm_name, wm_name, 256);
        free(wm_name);
    }
    if (wm_class) {
        strncpy(info->wm_class, wm_class, 256);
        free(wm_class);
    }
    info->wm_icon_data = wm_icon_data;
    info->wm_icon_data_size = size;
    unsigned long* ex = get_prop(client, XA_CARDINAL, "_NET_FRAME_EXTENTS",
                                NULL);
    if (ex) {
        info->extent.left = ex[0];
        info->extent.right = ex[1];
        info->extent.top = ex[2];
        info->extent.bottom = ex[3];
    }

    Window root;
    XGetGeometry(dpy, client, &root, &info->geometry.x, &info->geometry.y,
                 &info->geometry.width, &info->geometry.height,
                 &info->geometry.border_width, &info->geometry.depth);
}

void
ewmh_get_current_workarea(unsigned int* x, unsigned int* y,
                          unsigned int* width, unsigned int* height)
{
    long* data = get_prop(DefaultRootWindow(dpy), XA_CARDINAL,
                                  "_NET_WORKAREA", NULL);
    if (x) *x = data[0];
    if (y) *y = data[1];
    if (width) *width = data[2];
    if (height) *height = data[3];
    free(data);
}

window_type_t
ewmh_get_window_type(Window wnd)
{
    static const char* type_xas_name[] = {
        "_NET_WM_WINDOW_TYPE_DESKTOP",
        "_NET_WM_WINDOW_TYPE_DOCK",
        "_NET_WM_WINDOW_TYPE_TOOLBAR",
        "_NET_WM_WINDOW_TYPE_MENU",
        "_NET_WM_WINDOW_TYPE_UTILITY",
        "_NET_WM_WINDOW_TYPE_SPLASH",
        "_NET_WM_WINDOW_TYPE_DIALOG",
        "_NET_WM_WINDOW_TYPE_NORMAL"};
    static int type_xas_size = 8;
    
    Atom* res_xa = get_prop(wnd, XA_ATOM, "_NET_WM_WINDOW_TYPE", NULL);
    if (res_xa == NULL)
        return _NET_WM_WINDOW_TYPE_NORMAL;
    
    int i;
    for (i = 0; i < type_xas_size; i++) {
        if (XInternAtom(dpy, type_xas_name[i], true) == *res_xa) {
            free(res_xa);
            return i;
        }
    }
    free(res_xa);
    return _NET_WM_WINDOW_TYPE_MENU;
}

window_state_t*
ewmh_get_window_states(Window wnd, size_t* ret_sz)
{
    static const char* state_xas_name[] = {
        "_NET_WM_STATE_MODAL",
        "_NET_WM_STATE_STICKY",
        "_NET_WM_STATE_MAXIMIZED_VERT",
        "_NET_WM_STATE_MAXIMIZED_HORZ",
        "_NET_WM_STATE_SHADED",
        "_NET_WM_STATE_SKIP_TASKBAR",
        "_NET_WM_STATE_SKIP_PAGER",
        "_NET_WM_STATE_HIDDEN",
        "_NET_WM_STATE_FULLSCREEN",
        "_NET_WM_STATE_ABOVE",
        "_NET_WM_STATE_BELOW",
        "_NET_WM_STATE_DEMANDS_ATTENTION"};
    static const int state_xas_size = 12;
    size_t sz;
    Atom* res_xa = get_prop(wnd, XA_ATOM, "_NET_WM_STATE", &sz);
    int* res = malloc(sz * sizeof(int));
    int i, j;
    for (i = 0; i < sz; i++) {
        for (j = 0; j < state_xas_size; j++) {
            if (XInternAtom(dpy, state_xas_name[j], true) == res_xa[i])
                break;
        }
        res[i] = j;
    }
    free(res_xa);
    if (ret_sz) *ret_sz =sz;
    return res;
}
