#!/bin/sh
cd `dirname $0`
./cleanup.sh
echo "(This file was copied from README.md - don't edit this file directly)" > README
echo >> README
cat README.md >> README
autoreconf -i
./configure $@
