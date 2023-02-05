#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../.."
	DATAGEN="${TOPDIR}/builds/debug/src/dbt2-datagen"
	SEED=23

	mkdir -p "${SHUNIT_TMPDIR}/1"

	# Generate 1 warehouse.
	$DATAGEN -d "${SHUNIT_TMPDIR}/1" -w 1 --table warehouse --seed $SEED

	# Generate a 10 warehouses datafile and ten 1 warehouse data files.
	SCALE_FACTOR=10
	mkdir -p "${SHUNIT_TMPDIR}/${SCALE_FACTOR}"
	DATAFILE="${SHUNIT_TMPDIR}/${SCALE_FACTOR}/warehouse.data"
	$DATAGEN -d "${SHUNIT_TMPDIR}/${SCALE_FACTOR}" -w $SCALE_FACTOR \
			--table warehouse --seed $SEED
	for PART in 1 2 3 4 5 6 7 8 9 10; do
		$DATAGEN -d "${SHUNIT_TMPDIR}/${SCALE_FACTOR}" -w $SCALE_FACTOR \
				--table warehouse --seed $SEED -P $SCALE_FACTOR -p $PART
		cat "${DATAFILE}.${PART}" >> "${DATAFILE}.rebuilt"
	done

	# Generate a 100 warehouse datafile and two 50 warehouse data files.
	SCALE_FACTOR=100
	mkdir -p "${SHUNIT_TMPDIR}/${SCALE_FACTOR}"
	DATAFILE="${SHUNIT_TMPDIR}/${SCALE_FACTOR}/warehouse.data"
	$DATAGEN -d "${SHUNIT_TMPDIR}/${SCALE_FACTOR}" -w $SCALE_FACTOR \
			--table warehouse --seed $SEED
	$DATAGEN -d "${SHUNIT_TMPDIR}/${SCALE_FACTOR}" -w $SCALE_FACTOR \
			--table warehouse --seed $SEED -P 2 -p 1
	$DATAGEN -d "${SHUNIT_TMPDIR}/${SCALE_FACTOR}" -w $SCALE_FACTOR \
			--table warehouse --seed $SEED -P 2 -p 2
}

testSingleFileCardinality() {
	SCALE_FACTOR=1
	DATAFILE="${SHUNIT_TMPDIR}/1/warehouse.data"

	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" $SCALE_FACTOR "$COUNT"
}

testPartitionedFileByLine() {
	SCALE_FACTOR=10
	DATAFILE="${SHUNIT_TMPDIR}/${SCALE_FACTOR}/warehouse.data"

	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" $SCALE_FACTOR "$COUNT"

	diff "${DATAFILE}" "${DATAFILE}.rebuilt"
	assertEquals "match" 0 $?
}

testPartitionedFileSplit() {
	SCALE_FACTOR=100
	DATAFILE="${SHUNIT_TMPDIR}/${SCALE_FACTOR}/warehouse.data"

	COUNT=$(wc -l "${DATAFILE}" | cut -f 1 -d " ")
	assertEquals "cardinality" $SCALE_FACTOR "$COUNT"

	cat "${DATAFILE}.1" "${DATAFILE}.2" > "${DATAFILE}.rebuilt"
	diff "${DATAFILE}" "${DATAFILE}.rebuilt"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
