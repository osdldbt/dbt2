#!/bin/bash

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

while getopts "o:" opt; do
	case $opt in
	o)
		OUTPUT=$OPTARG
		;;
	esac
done

if [ "$OUTPUT" == "" ]; then
	echo "use -o and specify the backup file"
	exit 1;
fi

_test=`$PGRESTORE -v Fc -a -d $DB_NAME $OUTPUT | grep OK`
if [ "$_test" != "" ]; then
	echo "restore failed: $_test"
	exit 1
fi
