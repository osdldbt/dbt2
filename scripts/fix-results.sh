#!/bin/sh
#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2006 Mark Wong & Open Source Development Labs, Inc.
#

DIR=`dirname ${0}`
RESULTS=${1}
if [ ! -n "${RESULTS}" ]; then
	echo "Usage: ${0} <output directory>"
	exit 1
fi

if [ ! -d "${RESULTS}" ]; then
	echo "'${RESULTS}' is not a directory."
	exit 1
fi

LIST=`ls ${RESULTS}`
for i in ${LIST}; do
	if [ -d "${RESULTS}/${i}" ]; then
		DRIVER="${RESULTS}/${i}/driver"
		MIX="${DRIVER}/mix.log"
		if [ -f "${MIX}" ]; then
			${DIR}/mix_analyzer.pl --infile ${MIX} --outdir ${DRIVER} > ${DRIVER}/results.out
			${DIR}/gen_html.sh ${RESULTS}/${i}
		fi
	fi
done

exit 0
