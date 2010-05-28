from wm import *
from uiutils import *

wm = WindowManager()
for win in wm.list_windows():
    x, y = win.get_position()
    print("%s, at %d,%d type %s" % (win.title(), x, y, win.get_type()))
    print(win.get_states())
    info = win.get_info()
    if hasattr(info, 'icon_pixel_array'):
        pixbuf = create_pixbuf_from_icon_matrix(info.icon_pixel_array,
                                                info.icon_width,
                                                info.icon_height,
                                                32, 32)
        pixbuf.save(win.title() + '.png', 'png')
