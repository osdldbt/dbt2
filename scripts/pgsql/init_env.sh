#!/bin/sh

# init_env.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 01 May 2003

PATH=/usr/local/pgsql/bin:$PATH; export PATH

# Our database will be named dbt2
DB_NAME=dbt2; export DB_NAME

# Data instance path.  Storage location for this instance of the database
# should be within the working directory
PGDATA=/opt/pgdb; export PGDATA
