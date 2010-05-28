import gtk

def create_pixbuf_from_icon_array(pixel_data, width, height, dest_width, \
                                      dest_height):
    pixbuf = gtk.gdk.pixbuf_new_from_data(pixel_data, gtk.gdk.COLORSPACE_RGB,
                                          True, 8, width, height,
                                          4 * width)
    return pixbuf.scale_simple(dest_width, dest_height, gtk.gdk.INTERP_TILES)

def create_window_button(window, cb):
    info = window.get_info()
    btn = None
    if hasattr(info, 'icon_pixel_array'):
        pixbuf = create_pixbuf_from_icon_array(info.icon_pixel_array,
                                               info.icon_width,
                                               info.icon_height,
                                               22, 22)
        img_wid = gtk.image_new_from_pixbuf(pixbuf)
        btn = gtk.ToolButton(icon_widget=img_wid, label=window.title())
    else:
        btn = gtk.ToolButton(label=window.title())
    if cb is not None:
        btn.connect('clicked', cb)
    return btn

