#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

${PSQL} -e -d ${DBNAME} -c "CREATE INDEX i_orders ON orders (o_w_id, o_d_id, o_c_id);" || exit 1
${PSQL} -e -d ${DBNAME} -c "CREATE INDEX i_customer ON customer (c_w_id, c_d_id, c_last, c_first, c_id);" || exit 1

exit 0
