#!/bin/sh

# init_env.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 01 May 2003

. ./init_env.sh

dropdb $DB_NAME
rm -rf $PGDATA
