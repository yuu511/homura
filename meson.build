project('homura','cpp')
cc = meson.get_compiler('cpp')
homura_sources = ['src/main.cpp']

# dependencies 
dep_thread = dependency('threads')
dep_libtorrent = dependency('libtorrent-rasterbar')
dep_curl = dependency('libcurl')
# myhtml is usually not found in package repositories,
# default install library located in /usr/local/lib
# ( https://github.com/lexborisov/myhtml/blob/master/INSTALL.md )
dep_myhtml = cc.find_library('myhtml',dirs:['/usr/local/lib/'])
inc_myhtml = include_directories('/usr/local/lib')

all_dep = [ dep_thread, dep_libtorrent, dep_curl, dep_myhtml ]

# FLAGS

executable('homura', 
           sources: homura_sources,
           dependencies: all_dep,
           include_directories: inc_myhtml)
           
