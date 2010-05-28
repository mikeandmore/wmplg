#include <stdio.h>
#include <stdlib.h>

#include "wm.h"

int main(int argc, char *argv[])
{
    /* list all the xclients */
    init();
    size_t sz;
    Window* wins = ewmh_list_clients(&sz);
    int i;
    printf("%ld windows\n", sz);
    for (i = 0; i < sz; i++) {
        printf("0x%.8x ", (unsigned int) wins[i]);
        window_info_t info;
        ewmh_get_window_info(wins[i], &info);
        printf("name:%s class:%s ", info.wm_name, info.wm_class);
        printf("%d,%d+%ux%u (%u %u %u %u) bw %d\n",
               info.geometry.x, info.geometry.y,
               info.geometry.width, info.geometry.height,
               info.extent.left, info.extent.right,
               info.extent.top, info.extent.bottom,
               info.geometry.border_width);
        
        if (info.wm_icon_data)
            free(info.wm_icon_data);
    }
    free(wins);

    unsigned int x, y, width, height;
    ewmh_get_current_workarea(&x, &y, &width, &height);
    printf("\ncurrent working area %d %d %u %u\n", x, y, width, height);

    printf("active window is 0x%.2lx\n", ewmh_get_active_window());
    finalize();
    return 0;
}

