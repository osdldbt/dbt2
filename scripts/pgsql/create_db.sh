#!/bin/sh

# create_db.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 01 May 2003

DIR=`dirname $0`
. ${DIR}/init_env.sh || exit

# su to user postgres
# createuser -P $USERNAME
# answer yes to all questions
# where $USERNAME is the person you're trying to create

# Create database
echo "Creating database..."
if [ -d $PGDATA ] ; then
	echo "======================================="
	echo "PGData directory $PGDATA already exists"
	echo "Skipping initdb"
	echo "======================================="
else
	$INITDB -D $PGDATA
fi

$PGCTL -D $PGDATA -l log start

# Give the database a few seconds to get going
sleep 4

$CREATEDB $DB_NAME
$CREATELANG plpgsql $DB_NAME

