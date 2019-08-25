project('homura','cpp',default_options: ['cpp_std=c++11'])
cc = meson.get_compiler('cpp')
homura_sources = ['src/main.cpp','src/homura.cpp','src/errlib.cpp','src/magnet_table.cpp','src/curl_container.cpp','src/tree_container.cpp','src/url_table.cpp']

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

# homura assumes libcurl is built with OpenSSL.
# If you are using libcurl built with GNUTLS, pass the option -D GNUTLS=true.
if get_option('GNUTLS')
  add_project_arguments('-DGNUTLS', language: 'cpp')
  dep_GNUTLS = dependency('gnutls')
  all_dep += dep_GNUTLS
  # (debian-based) gcrypt does NOT have a .pc file.
  # write your own and uncomment below or manually link the library with path below
  # dep_CRYPTO = dependency('gcrypt')
  dep_CRYPTO = cc.find_library('libgcrypt',dirs:['/lib/x86_64-linux-gnu/'])
  all_dep += dep_CRYPTO
endif

# FLAGS
generic_flags = ['-g','-Wall']

e = executable('homura', 
                sources: homura_sources,
                dependencies: all_dep,
                include_directories: inc_myhtml,
                c_args : generic_flags)

# Tests
test ('default' , e, args : ['-v','dumbbell nan kilo moteru'])

