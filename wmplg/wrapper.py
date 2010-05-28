from ctypes import *
import weakref
from array import array

class WMWrapper(object):
    """
    libwm's wrapper. All FFI should goes here
    """

    class WMInfo(Structure):
        _fields_ = [('wm_name', c_char * 256),
                    ('wm_class', c_char * 256),
                    ('wm_pid', c_ulong),
                    ('wm_showing_desktop', c_ulong)]

    class WindowInfo(Structure):
        class _Extent(Structure):
            _fields_ = [('left', c_uint),
                        ('right', c_uint),
                        ('top', c_uint),
                        ('bottom', c_uint)]

        class _Geometry(Structure):
            _fields_ = [('x', c_int),
                        ('y', c_int),
                        ('width', c_uint),
                        ('height', c_uint),
                        ('border_width', c_uint),
                        ('depth', c_uint)]
        
        _anonymous_ = ('extent', 'geometry')
        _fields_ = [('wm_name', c_char * 256),
                    ('wm_class', c_char * 256),
                    ('icon_data', POINTER(c_ulong)),
                    ('icon_data_size', c_ulong),
                    ('extent', _Extent),
                    ('geometry', _Geometry)]
        
        def decode_icon(self):
            if not bool(self.icon_data) or self.icon_data_size == 0:
                return
            self.icon_width = self.icon_data[0]
            self.icon_height = self.icon_data[1]
            self.icon_pixel_array = array('B')
            for i in xrange(self.icon_height):
                for j in xrange(self.icon_width):
                    pixel = self.icon_data[2 + i * self.icon_width + j]
                    a = pixel >> 24 & 0x000000ff
                    r = (pixel >> 16) & 0x000000ff
                    g = (pixel >> 8) & 0x000000ff
                    b = pixel & 0x000000ff
                    self.icon_pixel_array.append(r)
                    self.icon_pixel_array.append(g)
                    self.icon_pixel_array.append(b)
                    self.icon_pixel_array.append(a)

    def __new__(cls):
        if not hasattr(cls, '_ref') or cls._ref() is None:
            ins = object.__new__(cls)
            cls._ref = weakref.ref(ins)
            ins._need_init = True
            return ins
        return cls._ref()
    
    def __init__(self):
        if self._need_init:
            self._need_init = False
        else:
            return
        self._libwm = CDLL('libwm.so.1')
        self._libc = CDLL('libc.so.6')
        self._libwm.ewmh_list_clients.restype = POINTER(c_ulong)
        self._libwm.ewmh_supports.restype = c_int
        self._libwm.ewmh_wm_info.restype = c_int
        self._libwm.ewmh_get_desktop.restype = c_ulong
        self._libwm.ewmh_get_current_desktop.restype = c_ulong
        self._libwm.ewmh_get_active_window.restype = c_ulong
        self._libwm.ewmh_get_window_type.restype = c_int
        self._libwm.ewmh_get_window_states.restype = POINTER(c_int)
        self._libwm.init()

    def __del__(self):
        self._libwm.finalize()

    def list_clients(self):
        sz_ret = c_ulong(0)
        wnd_array = self._libwm.ewmh_list_clients(byref(sz_ret))
        wnd_list = [wnd_array[i] for i in xrange(sz_ret.value)]
        self._libc.free(wnd_array)
        return wnd_list

    def get_window_info(self, wid):
        window = c_ulong(wid)
        info = WMWrapper.WindowInfo()
        self._libwm.ewmh_get_window_info(window, byref(info))
        info.decode_icon()
        self._libc.free(info.icon_data)
        return info

    def wm_supports(self, propname):
        res = self._libwm.ewmh_supports(c_char_p(propname))
        if res.value == 0:
            return False
        return True

    def wm_info(self):
        info = WMWrapper.WMInfo()
        res = self._libwm.ewmh_wm_info(byref(info))
        if res.value == 0:
            return None
        return res

    def get_desktop(self, wid):
        res = self._libwm.ewmh_get_desktop(c_ulong(wid))
        return res.value

    def get_current_desktop(self):
        res = self._libwm.ewmh_get_current_desktop()
        return res.value

    def set_current_desktop(self, desk_id):
        self._libwm.ewmh_set_current_desktop(c_ulong(desk_id))

    def get_active_window(self):
        return self._libwm.ewmh_get_active_window()

    def set_active_window(self, wid, switch_desktop=False):
        swflag = c_int(0)
        if switch_desktop:
            swflag = c_int(1)

        self._libwm.ewmh_set_active_window(c_ulong(wid), swflag)

    def get_current_workarea(self):
        x = c_uint()
        y = c_uint()
        width = c_uint()
        height = c_uint()

        self._libwm.ewmh_get_current_workarea(byref(x), byref(y),
                                              byref(width), byref(height))
        return (x.value, y.value, width.value, height.value)

    def move_window(self, wid, x, y):
        self._libwm.wm_frame_move(c_ulong(wid), c_int(x), c_int(y))

    def resize_window(self, wid, width, height):
        self._libwm.wm_frame_resize(c_ulong(wid),
                                    c_uint(width), c_uint(height))

    def get_window_type(self, wid):
        return self._libwm.ewmh_get_window_type(c_ulong(wid))

    def get_window_state(self, wid):
        sz = c_ulong()
        states_ptr = self._libwm.ewmh_get_window_states(c_ulong(wid),
                                                        byref(sz))
        states = [states_ptr[i] for i in xrange(sz.value)]
        self._libc.free(states_ptr)
        return states
    
