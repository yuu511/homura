project('homura','cpp',default_options: ['cpp_std=c++11'])
cc = meson.get_compiler('cpp')
homura_sources = ['src/main.cpp','src/homura.cpp','src/errlib.cpp']

# dependencies 
dep_thread = dependency('threads')
dep_libtorrent = dependency('libtorrent-rasterbar')
dep_curl = dependency('libcurl')
dep_boost = dependency('boost', modules : ['serialization'])
# note : myhtml is usually not found in package repositories,
# install here: https://github.com/lexborisov/myhtml/blob/master/INSTALL.md 
dep_myhtml = cc.find_library('myhtml',dirs:['/usr/local/lib/'])
inc_myhtml = include_directories('/usr/local/lib')

all_dep = [ dep_thread, dep_libtorrent, dep_curl, dep_myhtml, dep_boost ]

# FLAGS
generic_flags = ['-g','-Wall']

executable('homura', 
           sources: homura_sources,
           dependencies: all_dep,
           include_directories: inc_myhtml,
           c_args : generic_flags)
