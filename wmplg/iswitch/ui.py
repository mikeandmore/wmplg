from wmplg.wm import *
from wmplg.uiutils import *

import keybinder
import re
import gtk
import gobject

class ISwitchWindow(gtk.Window):
    def __init__(self):
        super(ISwitchWindow, self).__init__()
        self.set_skip_pager_hint(True)
        self.set_skip_taskbar_hint(True)
        self.set_size_request(300, 400)
        self.set_type_hint(gtk.gdk.WINDOW_TYPE_HINT_UTILITY)
        self.set_keep_above(True)
        self.set_title('iswitch')
        
        self._toolbar = gtk.Toolbar()
        self._toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
        
        self._input = gtk.Entry()
        self._input.connect('key-press-event', self.on_key_pressed)
        
        vbox = gtk.VBox()
        vbox.pack_start(self._input, fill=False, expand=False)
        vbox.pack_start(self._toolbar)
        self.add(vbox)
        self.connect('delete-event', self.on_delete)

        self.need_refresh = False
        gobject.timeout_add(600, self.refresh)
        

        self.wm = WindowManager()

    def filter_all_windows(self, text):
        from datetime import datetime
        a = datetime.now()
        pattern = None
        try:
            if text != '':
                pattern = re.compile(text, re.IGNORECASE)
        except:
            pass
        
        self.filtered_windows = []
        for w in self.wm.list_windows():
            if w.get_type() != WindowType.NORMAL:
                continue
            if pattern and pattern.search(w.title()) is None:
                continue
            self.filtered_windows.append(w)
        
    def add_filtered_windows(self):
        # clear the button box
        self._toolbar.foreach(lambda wid: \
                                     self._toolbar.remove(wid))
        for w in self.filtered_windows:
            def _add_window_btn(win):
                btn = create_window_button(win, lambda *args: \
                                               self.switch_to(win))
                self._toolbar.insert(btn, -1)
            _add_window_btn(w)
            
    def refresh(self):
        if not self.need_refresh:
            return True
        
        self.alive = True
        text = self._input.get_text()
        self.filter_all_windows(text)
        self.add_filtered_windows()
        self.show_all();

        time = keybinder.get_current_event_time()
        self.present_with_time(time)
        self.need_refresh = False
        return True
        
    def switch_to(self, w):
        self.wm.set_current_window(w)
        self.do_hide()
        
    def do_hide(self):
        self._input.set_text('')
        self.alive = False
        self.need_refresh = False
        self.hide()

    def on_delete(self, *args):
        self.hide()
        return True
    
    def on_key_pressed(self, wid, evt, *args):
        # enter key
        if evt.keyval == 0xff0d and len(self.filtered_windows) > 0:
            self.switch_to(self.filtered_windows[0])
            return

        if evt.keyval == 0xff1b:
            self.do_hide()
            return
        
        if self.alive:
            self.need_refresh = True

w = ISwitchWindow()

def fire_iswitch():
    w.need_refresh = True
    w.refresh()
