#!/bin/sh
# Recommended options for typical situations:
# 1) Static linking only: ./autogen.sh --disable-shared
# 2) Debug mode with g++: CPPFLAGS=-DDEBUG CXXFLAGS="-g -O0" ./autogen.sh --disable-shared
cd `dirname $0`
./cleanup.sh
echo "(This file was copied from README.md - don't edit this file directly)" > README
echo >> README
cat README.md >> README
autoreconf -i
./configure $@
