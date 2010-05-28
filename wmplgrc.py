from keybinder import bind, unbind
from wmplg.windowkey import *
from wmplg.iswitch import *

bind('<super>h', lambda : vertical_expand_current_window(760))
bind('<super>leftarrow', lambda: move_current_window(-10, 0))
bind('<super>rightarrow', lambda: move_current_window(10, 0))
bind('<super>uparrow', lambda: move_current_window(0, -10))
bind('<super>downarrow', lambda: move_current_window(0, 10))
bind('<super>s', fire_iswitch)



