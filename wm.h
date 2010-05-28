#ifndef _WM_H_
#define _WM_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char          wm_name[256]; /**< wm name */
    char          wm_class[256]; /**< wm class */
    unsigned long wm_pid; /**< wm pid, this might be 0 for get failure */
    unsigned long wm_showing_desktop;
} wminfo_t;

typedef struct {
    char wm_name[256];
    char wm_class[256];
    unsigned long* wm_icon_data; /**< needs to be freed when dispose */
    size_t wm_icon_data_size;
    struct {
        unsigned int left, right, top, bottom;
    } extent;
    struct {
        int x, y;
        unsigned int width, height;
        unsigned int border_width, depth;
    } geometry;
} window_info_t;

typedef unsigned long desktop_id_t;

typedef int window_type_t;

/* window types */
#define _NET_WM_WINDOW_TYPE_DESKTOP 0
#define _NET_WM_WINDOW_TYPE_DOCK    1
#define _NET_WM_WINDOW_TYPE_TOOLBAR 2
#define _NET_WM_WINDOW_TYPE_MENU    3
#define _NET_WM_WINDOW_TYPE_UTILITY 4
#define _NET_WM_WINDOW_TYPE_SPLASH  5
#define _NET_WM_WINDOW_TYPE_DIALOG  6
#define _NET_WM_WINDOW_TYPE_NORMAL  7

typedef int window_state_t;

/* window state */
#define _NET_WM_STATE_MODAL             0
#define _NET_WM_STATE_STICKY            1
#define _NET_WM_STATE_MAXIMIZED_VERT    2
#define _NET_WM_STATE_MAXIMIZED_HORZ    3
#define _NET_WM_STATE_SHADED            4
#define _NET_WM_STATE_SKIP_TASKBAR      5
#define _NET_WM_STATE_SKIP_PAGER        6
#define _NET_WM_STATE_HIDDEN            7
#define _NET_WM_STATE_FULLSCREEN        8
#define _NET_WM_STATE_ABOVE             9
#define _NET_WM_STATE_BELOW             10
#define _NET_WM_STATE_DEMANDS_ATTENTION 11
    

void          init();
void          finalize();

void          wm_frame_move            (Window wmframe, int x, int y);
void          wm_frame_resize          (Window wmframe, unsigned int w,
                                        unsigned int h);

bool          ewmh_supports            (const char* prop);
bool          ewmh_wm_info             (wminfo_t* info);
Window*       ewmh_list_clients        (size_t* sz_ret);
desktop_id_t  ewmh_get_desktop         (Window client);
desktop_id_t  ewmh_get_current_desktop (void);
void          ewmh_set_current_desktop (desktop_id_t desk);
void          ewmh_set_active_window   (Window client, bool switch_desktop);
Window        ewmh_get_active_window   (void);
void          ewmh_get_window_info     (Window client, window_info_t* info);
void          ewmh_get_current_workarea(unsigned int* x,
                                        unsigned int* y,
                                        unsigned int* width,
                                        unsigned int* height);

window_type_t   ewmh_get_window_type     (Window wnd);
window_state_t* ewmh_get_window_states   (Window wnd, size_t* ret_sz);

#endif /* _WM_H_ */
