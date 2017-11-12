#!/bin/sh

make -k clean >/dev/null 2>&1
make -k distclean >/dev/null 2>&1
find . -name Makefile.in -exec rm -f {} \;
rm -rf \
    Makefile \
    Makefile.in \
    README \
    aclocal.m4 \
    ar-lib \
    autom4te.cache \
    compile \
    configure \
    configure.in \
    config.guess \
    config.h.in \
    config.sub \
    config.log \
    depcomp \
    install-sh \
    ltmain.sh \
    missing \
    test-driver

