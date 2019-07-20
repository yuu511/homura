project('homura','cpp')
thread_dep = dependency('threads')
libtorrent_dep = dependency('libtorrent-rasterbar')
homura_sources = ['src/main.cpp']

all_dep = [thread_dep,
           libtorrent_dep]

executable('homura', 
           sources: homura_sources,
           dependencies: all_dep)
