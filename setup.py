#!/usr/bin/env python

from distutils.core import setup

setup(name='wmplg',
      version='0.0.1',
      description='Window Manager plugins',
      author='Mike Qin',
      author_email='mikeandmore@gmail.com',
      url='http://mike.struct.cn',
      packages=['wmplg', 'wmplg.windowkey', 'wmplg.iswitch'],
      scripts=['scripts/wmplg']
      )
