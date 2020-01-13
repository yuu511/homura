# Homura

## WHAT:
Personal project that scrapes torrents from nyaa.si (other websites coming soon)

## Build:
```
meson setup builddir
ninja -c builddir
```

Dependencies: 

libcurl
myHTML (https://github.com/lexborisov/myhtml)
libtorrent
pthreads

# WHY:
so output can be used with other shell utilities (I need grep in my life)
