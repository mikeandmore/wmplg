from wmplg import WindowManager

def move_current_window(dx, dy):
    wm = WindowManager()
    win = wm.current_window()
    x, y = win.get_position()
    x += dx
    y += dy
    win.move(x, y)

def vertical_expand_current_window(height):
    wm = WindowManager()
    win = wm.current_window()
    x, y = win.get_position()
    w, h = win.get_size()
    win.move(x, 0)
    win.resize(w, height)

