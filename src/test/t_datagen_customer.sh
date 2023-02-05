#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../.."
	DATAGEN="${TOPDIR}/builds/debug/src/dbt2-datagen"
	DATAFILE="${SHUNIT_TMPDIR}/customer.data"
	COLUMNS="1,2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18,19,20,21"
}

testSingleFile() {
	SCALE_FACTOR=1

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table customer
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 30000 "$COUNT"
}

testPartitionedFileSplit() {
	SEED=3
	SCALE_FACTOR=10

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table customer \
			--seed $SEED
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 300000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table customer \
			--seed $SEED -P 2 -p 1
	COUNT=$(wc -l "${DATAFILE}.1" | cut -f 1 -d " ")
	assertEquals "top half" 150000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table customer \
			--seed $SEED -P 2 -p 2
	COUNT=$(wc -l "${DATAFILE}.2" | cut -f 1 -d " ")
	assertEquals "bottom half" 150000 "$COUNT"

	# Strip out timestamp column that is not stable, changes depending on when
	# data is created.

	cut -f "$COLUMNS" "${DATAFILE}" > "${SHUNIT_TMPDIR}/c.orig"

	cut -f "$COLUMNS" "${DATAFILE}.1" > "${SHUNIT_TMPDIR}/c.new"
	cut -f "$COLUMNS" "${DATAFILE}.2" >> "${SHUNIT_TMPDIR}/c.new"

	diff "${SHUNIT_TMPDIR}/c.orig" "${SHUNIT_TMPDIR}/c.new"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
