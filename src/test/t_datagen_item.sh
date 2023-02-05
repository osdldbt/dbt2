#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../.."
	DATAGEN="${TOPDIR}/builds/debug/src/dbt2-datagen"
	DATAFILE="${SHUNIT_TMPDIR}/item.data"
}

testSingleFile() {
	SCALE_FACTOR=1

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table item
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 100000 "$COUNT"
}

testPartitionedFileSplit() {
	SEED=9
	SCALE_FACTOR=10

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table item \
			--seed $SEED
	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" 100000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table item \
			--seed $SEED -P 2 -p 1
	COUNT=$(wc -l "${DATAFILE}.1" | cut -f 1 -d " ")
	assertEquals "top half" 50000 "$COUNT"

	$DATAGEN -d "$SHUNIT_TMPDIR" -w $SCALE_FACTOR --table item \
			--seed $SEED -P 2 -p 2
	COUNT=$(wc -l "${DATAFILE}.2" | cut -f 1 -d " ")
	assertEquals "bottom half" 50000 "$COUNT"

	cat "${DATAFILE}.1" "${DATAFILE}.2" > "${DATAFILE}.rebuilt"
	diff "${DATAFILE}" "${DATAFILE}.rebuilt"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
