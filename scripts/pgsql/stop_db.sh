#!/bin/sh

# start_db.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Rod Taylor & Open Source Development Lab, Inc.
#
# 15 May 2003

DIR=`dirname $0`

. ${DIR}/init_env.sh || exit

pg_ctl -D $PGDATA stop
