#!/bin/sh

# start_db.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Rod Taylor & Open Source Development Lab, Inc.
#
# 15 May 2003

FLAG=$1

DIR=`dirname $0`
. ${DIR}/init_env.sh || exit

# We need the sleeps just in case we start pg_ctl commands too closely
# together.  Only start pg_autovacuum if explicitly called.

sleep 1
$PGCTL -D $PGDATA -l log start
sleep 2
if [ ! -z $FLAG ]; then
	echo Waiting for database to start before starting pg_autovacuum...
	sleep 10
	$PG_AUTOVACUUM -D
fi
