#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../.."
	DATAGEN="${TOPDIR}/builds/debug/src/dbt2-datagen"
	DATAFILE="${SHUNIT_TMPDIR}/history.data"
	COLUMNS="1,2,3,4,5,7,8"
}

testSingleFile() {
	SCALE_FACTOR=1

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table history
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 30000 "$COUNT"
}

testPartitionedFileSplit() {
	SEED=8
	SCALE_FACTOR=10

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table history \
			--seed $SEED
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 300000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table history \
			--seed $SEED -P 2 -p 1
	COUNT=$(wc -l "${DATAFILE}.1" | cut -f 1 -d " ")
	assertEquals "top half" 150000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table history \
			--seed $SEED -P 2 -p 2
	COUNT=$(wc -l "${DATAFILE}.2" | cut -f 1 -d " ")
	assertEquals "bottom half" 150000 "$COUNT"

	# Strip out timestamp column that is not stable, changes depending on when
	# data is created.

	cut -f "$COLUMNS" "${DATAFILE}" > "${SHUNIT_TMPDIR}/h.orig"

	cut -f "$COLUMNS" "${DATAFILE}.1" > "${SHUNIT_TMPDIR}/h.new"
	cut -f "$COLUMNS" "${DATAFILE}.2" >> "${SHUNIT_TMPDIR}/h.new"

	diff "${SHUNIT_TMPDIR}/h.orig" "${SHUNIT_TMPDIR}/h.new"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
