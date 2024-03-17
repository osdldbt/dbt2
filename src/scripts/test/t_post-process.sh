#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../../.."
	ACTUALOUTPUT="${SHUNIT_TMPDIR}/summary.rst"
	EXPECTEDOUTPUT="${TOPDIR}/src/scripts/test/summary.rst.expected"
	export PATH="${TOPDIR}/builds/debug:${TOPDIR}/src/scripts:${PATH}"
}

testPostProcessR() {
	# shellcheck disable=SC2086
	find ${TOPDIR}/src/scripts/test -name "mix-*.log" -print0 | \
			xargs -0 ${TOPDIR}/src/scripts/dbt2-post-process.r \
			> "$ACTUALOUTPUT"
	diff "$ACTUALOUTPUT" "$EXPECTEDOUTPUT"
	assertEquals "match" 0 $?
}

testPostProcessSQLite() {
	# shellcheck disable=SC2086
	find ${TOPDIR}/src/scripts/test -name "mix-*.log" -print0 | \
			xargs -0 ${TOPDIR}/builds/debug/dbt2-post-process.sqlite3 \
			> "$ACTUALOUTPUT"
	diff "$ACTUALOUTPUT" "$EXPECTEDOUTPUT"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
