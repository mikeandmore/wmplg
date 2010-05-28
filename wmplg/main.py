import os
import imp
import gtk

def load_config():
    path = os.path.expanduser('~/.wmplgrc')
    imp.load_source('*', path)
    
def main():
    load_config()
    gtk.main()
