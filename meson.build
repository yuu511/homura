project('homura','cpp',default_options: ['cpp_std=c++17'])
cc = meson.get_compiler('cpp')

# dependencies 
dep_thread = dependency('threads')
dep_curl = dependency('libcurl')
dep_boost = dependency('boost', modules : ['serialization'])
# note : myhtml is usually not found in package repositories,
# install here: https://github.com/lexborisov/myhtml/blob/master/INSTALL.md 
dep_myhtml = cc.find_library('myhtml',dirs:['/usr/local/lib/'])
inc_myhtml = include_directories('/usr/local/lib')

all_dep = [ dep_thread, dep_curl, dep_myhtml, dep_boost ]

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
else
  dep_OPENSSL = dependency('openssl')
  all_dep += dep_OPENSSL
endif

DEBUG = get_option('DEBUG')

if (DEBUG == 1)
  add_project_arguments('-DDEBUG=1', language: 'cpp')
elif (DEBUG == 2)
  add_project_arguments('-DDEBUG=2', language: 'cpp')
endif

# FLAGS
generic_flags = ['-g','-Wall']

# Sources
homura_sources = ['src/homura.cpp', 'src/errlib.cpp', 'src/curl_container.cpp'
                   , 'src/tree_container.cpp', 'src/url_table.cpp','src/url_scheduler.cpp', 'src/nyaasi_extractor.cpp']

homura_client = homura_sources + ['src/main.cpp']
homura_daemon = homura_sources + ['src/daemon_main.cpp']

homura_include = include_directories('include')
all_includes = [homura_include,inc_myhtml]

homurac = executable('homura' ,
                      sources: homura_client,
                      dependencies: all_dep,
                      include_directories: all_includes, 
                      c_args: generic_flags)

# homurad = executable('homurad' ,
#                       sources: homura_daemon,
#                       dependencies: all_dep,
#                       include_directories: all_includes, 
#                       c_args: generic_flags)

# Tests
test ('default' , homurac, args : ['-v','search','dumbbell nan kilo moteru'])

