from wrapper import WMWrapper

class WindowType(object):

    _repr_string_ = ['WINDOW_TYPE_DESKTOP',
                     'WINDOW_TYPE_DOCK',
                     'WINDOW_TYPE_TOOLBAR',
                     'WINDOW_TYPE_MENU',
                     'WINDOW_TYPE_UTILITY',
                     'WINDOW_TYPE_SPLASH',
                     'WINDOW_TYPE_DIALOG',
                     'WINDOW_TYPE_NORMAL']
    
    def __init__(self, typeid):
        self._typeid = typeid

    def __eq__(self, obj):
        if hasattr(obj, '_typeid'):
            return (self._typeid == obj._typeid)
        return False

    def __ne__(self, obj):
        return not self.__eq__(obj)

    def __repr__(self):
        return self._repr_string_[self._typeid]

WindowType.DESKTOP = WindowType(0)
WindowType.DOCK = WindowType(1)
WindowType.TOOLBAR = WindowType(2)
WindowType.MENU = WindowType(3)
WindowType.UTILITY = WindowType(4)
WindowType.SPLASH = WindowType(5)
WindowType.DIALOG = WindowType(6)
WindowType.NORMAL = WindowType(7)

class WindowState(object):

    _repr_string_ = ['WINDOW_STATE_MODAL',
                     'WINDOW_STATE_STICKY',
                     'WINDOW_STATE_MAXIMIZED_VERT',
                     'WINDOW_STATE_MAXIMIZED_HORZ',
                     'WINDOW_STATE_SHADED',
                     'WINDOW_STATE_SKIP_TASKBAR',
                     'WINDOW_STATE_SKIP_PAGER',
                     'WINDOW_STATE_HIDDEN',
                     'WINDOW_STATE_FULLSCREEN',
                     'WINDOW_STATE_ABOVE',
                     'WINDOW_STATE_BELOW',
                     'WINDOW_STATE_DEMANDS_ATTENTION']

    def __init__(self, infoid):
        self._infoid = infoid

    def __eq__(self, obj):
        if hasattr('_infoid', obj):
            return (self._infoid == obj._infoid)
        return False

    def __ne__(self, obj):
        return not self.__eq__(obj)

    def __repr__(self):
        return self._repr_string_[self._infoid]

WindowState.MODAL = WindowState(0)
WindowState.STICKY = WindowState(1)
WindowState.MAXIMIZED_VERT = WindowState(2)
WindowState.MAXIMIZED_HORZ = WindowState(3)
WindowState.SHADED = WindowState(4)
WindowState.SKIP_TASKBAR = WindowState(5)
WindowState.SKIP_PAGER = WindowState(6)
WindowState.HIDDEN = WindowState(7)
WindowState.FULLSCREEN = WindowState(8)
WindowState.ABOVE = WindowState(9)
WindowState.BELOW = WindowState(10)
WindowState.DEMANDS_ATTENTION = WindowState(11)

class Window(object):
    
    '''
    Window for X clients
    '''
    
    def __init__(self, wid):
        '''
        Arguments:
        - `wid`: window id.
        '''
        self._wid = wid
        self._wrapper_ = WMWrapper()

    def get_info(self):
        return self._wrapper_.get_window_info(self._wid)

    def get_position(self):
        info = self.get_info()
        area_x, area_y, area_width, area_height = \
            self._wrapper_.get_current_workarea()
        return (info.geometry.x - info.extent.left - area_x,
                info.geometry.y - info.extent.top - area_y)

    def get_size(self):
        info = self.get_info()
        return (info.geometry.width + info.extent.left + info.extent.right,
                info.geometry.height + info.extent.top + info.extent.bottom)

    def move(self, x, y):
        self._wrapper_.move_window(self._wid, x, y)

    def resize(self, width, height):
        info = self.get_info()
        self._wrapper_.resize_window(self._wid, width - info.extent.left \
                                         - info.extent.right,
                                     height - info.extent.top \
                                         - info.extent.bottom)

    def title(self):
        info = self.get_info()
        return info.wm_name

    def wm_class(self):
        info = self.get_info()
        return info.wm_class

    def get_type(self):
        return WindowType(self._wrapper_.get_window_type(self._wid))

    def get_states(self):
        return [WindowState(stateid) for stateid in \
                    self._wrapper_.get_window_state(self._wid)]

    def __repr__(self):
        return self.title()

class WindowManager(object):
    def __init__(self):
        self._wrapper_ = WMWrapper()

    def current_window(self):
        return Window(self._wrapper_.get_active_window())

    def set_current_window(self, win, switch_desktop=False):
        return self._wrapper_.set_active_window(win._wid, switch_desktop)

    def list_windows(self):
        return [Window(wid) for wid in self._wrapper_.list_clients()]

    def current_desktop(self):
        return self._wrapper_.get_current_desktop()

    def set_current_desktop(self, desk_id):
        return self._wrapper_.set_current_desktop(desk_id)

