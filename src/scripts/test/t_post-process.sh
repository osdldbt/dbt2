#!/bin/sh

oneTimeSetUp() {
	THISDIR=$(dirname "$0")
	TOPDIR="${THISDIR}/../../.."
	POSTPROCESS="${TOPDIR}/src/scripts/dbt2-post-process"
	ACTUALOUTPUT="${SHUNIT_TMPDIR}/summary.rst"
	EXPECTEDOUTPUT="${TOPDIR}/src/scripts/test/summary.rst.expected"
	export PATH="${TOPDIR}/builds/debug:${TOPDIR}/src/scripts:${PATH}"
}

testPostProcessR() {
	export DBT2LANG="R"
	# shellcheck disable=SC2086
	$POSTPROCESS ${TOPDIR}/src/scripts/test/mix-*.log > "$ACTUALOUTPUT"
	diff "$ACTUALOUTPUT" "$EXPECTEDOUTPUT"
	assertEquals "match" 0 $?
}

testPostProcessSQLite() {
	export DBT2LANG="sqlite3"
	# shellcheck disable=SC2086
	$POSTPROCESS ${TOPDIR}/src/scripts/test/mix-*.log > "$ACTUALOUTPUT"
	diff "$ACTUALOUTPUT" "$EXPECTEDOUTPUT"
	assertEquals "match" 0 $?
}

# shellcheck source=/dev/null
. "$(which shunit2)"
