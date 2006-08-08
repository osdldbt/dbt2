#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2004 Mark Wong & Open Source Development Lab, Inc.
#

if [ $# -ne 1 ]; then
	echo "usage: gen_html.sh <results directory>"
fi

DIR=$1

echo '<html>' > ${DIR}/index.html

echo '<head>' >> ${DIR}/index.html
echo '<title>DBT-2 PostgreSQL Results</title>' >> ${DIR}/index.html
echo '</head>' >> ${DIR}/index.html

echo '<body>' >> ${DIR}/index.html

echo '<pre>' >> ${DIR}/index.html
cat ${DIR}/readme.txt >> ${DIR}/index.html
echo '</pre>' >> ${DIR}/index.html
echo '<hr/>' >> ${DIR}/index.html

echo '<pre>' >> ${DIR}/index.html
cat ${DIR}/driver/results.out >> ${DIR}/index.html
echo '</pre>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'System Statistics<br/>' >> ${DIR}/index.html
echo '<a href="iostatx.out">iostat</a><br/>' >> ${DIR}/index.html
echo '<a href="vmstat.out">vmstat</a> [<a href="vmcharts.html">charts</a>]<br/>' >> ${DIR}/index.html
echo '<a href="sar.out">sar</a><br/>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'Kernel and Application Profiles<br/>' >> ${DIR}/index.html
echo '<a href="readprofile_ticks.txt">readprofile</a><br/>' >> ${DIR}/index.html
echo '<a href="oprofile.txt">oprofile</a><br/>' >> ${DIR}/index.html
echo '<a href="callgraph.txt">callgraph</a><br/>' >> ${DIR}/index.html
#echo '<a href="oprofile/current">oprofile raw data</a><br/>' >> ${DIR}/index.html
echo '<a href="oprofile/annotate">opannotate source output</a><br/>' >> ${DIR}/index.html
echo '<a href="oprofile/assembly.txt">opannotate assembly output</a><br/>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'Operating System Information<br/>' >> ${DIR}/index.html
echo '<a href="proc.out">linux /proc</a><br/>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'Database Information<br/>' >> ${DIR}/index.html
echo '<a href="db/param.out">database parameters</a><br/>' >> ${DIR}/index.html
echo '<a href="db/plan0.out">explain plans</a><br/>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'Test Output Files<br/>' >> ${DIR}/index.html
echo '<a href="client">client output</a><br/>' >> ${DIR}/index.html
echo '<a href="driver">driver output</a><br/>' >> ${DIR}/index.html
echo '<a href="db">database output</a><br/>' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'New-Order Transaction per Minutes<br/>' >> ${DIR}/index.html
echo '<img src="./driver/notpm.png" />' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo 'Processor Utilization<br/>' >> ${DIR}/index.html
echo '<img src="plots/cpu.png" />' >> ${DIR}/index.html

echo '<hr/>' >> ${DIR}/index.html

echo '<a href="db.html">Database Charts</a><br/>' >> ${DIR}/index.html

# Database Charts
echo '<html>' > ${DIR}/db.html
echo '<head>' >> ${DIR}/db.html
echo '<title>DBT-2 PostgreSQL Database Charts</title>' >> ${DIR}/db.html
echo '</head>' >> ${DIR}/db.html
echo '<body>' >> ${DIR}/db.html
echo 'Index Scans<br/>' >> ${DIR}/db.html
echo '<img src="./db/indexes_scan.png" />' >> ${DIR}/db.html

echo '<hr/>' >> ${DIR}/db.html

echo 'Index Blocks Read<br/>' >> ${DIR}/db.html
echo '<img src="./db/index_info.png" />' >> ${DIR}/db.html

echo '<hr/>' >> ${DIR}/db.html

echo 'Table Blocks Read<br/>' >> ${DIR}/db.html
echo '<img src="./db/table_info.png" />' >> ${DIR}/db.html
echo '</body>' >> ${DIR}/db.html
echo '</html>' >> ${DIR}/db.html

echo '<hr/>' >> ${DIR}/index.html

echo '<a href="rt.html">Response Time Charts</a><br/>' >> ${DIR}/index.html

# Response Time Charts
echo '<html>' > ${DIR}/rt.html
echo '<head>' >> ${DIR}/rt.html
echo '<title>DBT-2 PostgreSQL Response Time Charts</title>' >> ${DIR}/rt.html
echo '</head>' >> ${DIR}/rt.html
echo '<body>' >> ${DIR}/rt.html
echo 'Delivery Response Time Distribution<br/>' >> ${DIR}/rt.html
echo '<img src="driver/delivery.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'New-Order Response Time Distribution<br/>' >> ${DIR}/rt.html
echo '<img src="driver/new_order.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Order-Status Response Time Distribution<br/>' >> ${DIR}/rt.html
echo '<img src="driver/order_status.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Payment Response Time Distribution<br/>' >> ${DIR}/rt.html
echo '<img src="driver/payment.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Stock-Level Response Time Distribution<br/>' >> ${DIR}/rt.html
echo '<img src="driver/stock_level.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Delivery Response Time<br/>' >> ${DIR}/rt.html
echo '<img src="driver/d_tran_bar.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'New-Order Response Time<br/>' >> ${DIR}/rt.html
echo '<img src="driver/n_tran_bar.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Order-Status Response Time<br/>' >> ${DIR}/rt.html
echo '<img src="driver/o_tran_bar.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Payment Response Time<br/>' >> ${DIR}/rt.html
echo '<img src="driver/p_tran_bar.png" />' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/rt.html

echo 'Stock-Level Response Time<br/>' >> ${DIR}/rt.html
echo '<img src="driver/s_tran_bar.png" />' >> ${DIR}/rt.html
echo '</body>' >> ${DIR}/rt.html
echo '</html>' >> ${DIR}/rt.html

echo '<hr/>' >> ${DIR}/index.html
echo '</body>' >> ${DIR}/index.html
echo '</html>' >> ${DIR}/index.html

# vmstat Charts

echo "<html>" > ${DIR}/vmcharts.html
echo "<head>" >> ${DIR}/vmcharts.html
echo "<title>Database Test 2 Result vmstat Charts</title>" >> ${DIR}/vmcharts.html
echo "</head>" >> ${DIR}/vmcharts.html
echo "<body>" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/cpu.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/cs.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/in.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/io.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/memory.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/procs.png\" /><br />" >> ${DIR}/vmcharts.html
echo "<hr />" >> ${DIR}/vmcharts.html
echo "<img src=\"plots/swap.png\" /><br />" >> ${DIR}/vmcharts.html
echo "</body>" >> ${DIR}/vmcharts.html
echo "</html>" >> ${DIR}/vmcharts.html
