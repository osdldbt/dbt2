#!/bin/bash

# developer: customize this file with your local setup. The following commands
# will generate configure script for you from configure.in. And make dist will
# make a distribution tarball for you to test.

export PATH=/scratch/pgsql/bin:$PATH

aclocal || exit
autoheader || exit
autoconf || exit
automake || exit

./configure --with-prefix=/scratch/pgsql --with-getopt=/s/getopt-0 --with-postgresql=/scratch/pgsql || exit
make dist


