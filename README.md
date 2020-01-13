# Homura
Personal project that scrapes torrents from nyaa.si (other websites coming soon)

## Build:
```
meson setup builddir
ninja -C builddir
```

Dependencies: 

* libcurl 
* myHTML (https://github.com/lexborisov/myhtml)
* libtorrent
* pthreads


libcurl is expected to be compiled with openSSL.
if compiled with gnutls, please add option GNUTLS to meson: 
```
meson setup -DGNUTLS builddir
```
