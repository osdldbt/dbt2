#!/bin/sh

echo '<html>' > index.html
echo '	<head>' >> index.html
echo '		<title>DBT-2 PostgreSQL Data</title>' >> index.html
echo '	</head>' >> index.html
echo '	<body>' >> index.html
echo '		<pre>' >> index.html
cat driver/results.out >> index.html
echo '		</pre>' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		<a href="vmstat.out">vmstat</a><br/>' >> index.html
sar -A -f sar_raw.out > sar.out
echo '		<a href="sar.out">sar</a><br/>' >> index.html
echo '		<a href="readprofile_ticks.out">readprofile</a><br/>' >> index.html
echo '		<a href="oprofile.txt">oprofile</a><br/>' >> index.html
head -3 oprofile.txt > oprofile_postgres.txt
grep postgres oprofile.txt >> oprofile_postgres.txt
echo '		<a href="oprofile_postgres.txt">postgres oprofile</a><br/>' >> index.html
echo '		<a href="proc.out">linux /proc</a><br/>' >> index.html
echo '		<a href="db/plan0.out">explain plans</a><br/>' >> index.html
echo '		<a href="db/param.out">database parameters</a><br/>' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		<a href="client">client output</a><br/>' >> index.html
echo '		<a href="driver">driver output</a><br/>' >> index.html
echo '		<a href="db">database output</a><br/>' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		New-Order Transaction per Minutes<br/>' >> index.html
echo '		<img src="./driver/not.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Processor Utilization<br/>' >> index.html
echo '		<img src="./cpu.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Index Scans<br/>' >> index.html
echo '		<img src="./db/index_scans.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Index Blocks Read<br/>' >> index.html
echo '		<img src="./db/index_info.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Table Blocks Read<br/>' >> index.html
echo '		<img src="./db/table_info.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Transactions per Second for Table/Index Data<br/>' >> index.html
echo '		<img src="./data.tps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read for Table/Index Data<br/>' >> index.html
echo '		<img src="./data.blkread.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read per Second for Table/Index Data<br/>' >> index.html
echo '		<img src="./data.blkreadps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written for Table/Index Data<br/>' >> index.html
echo '		<img src="./data.blkwrtn.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written per Second for Table/Index Data<br/>' >> index.html
echo '		<img src="./data.blkwrtnps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Transactions per Second for Database Log<br/>' >> index.html
echo '		<img src="./log_tps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read for Database Log<br/>' >> index.html
echo '		<img src="./log_blkread.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read per Second for Database Log<br/>' >> index.html
echo '		<img src="./log_blkreadps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written for Database Log<br/>' >> index.html
echo '		<img src="./log_blkwrtn.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written per Second for Database Log<br/>' >> index.html
echo '		<img src="./log_blkwrtnps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Transactions per Second for Database Temp Space<br/>' >> index.html
echo '		<img src="./temp.tps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read for Database Temp Space<br/>' >> index.html
echo '		<img src="./temp.blkread.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Read per Second for Database Temp Space<br/>' >> index.html
echo '		<img src="./temp.blkreadps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written for Database Temp Space<br/>' >> index.html
echo '		<img src="./temp.blkwrtn.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '		Blocks Written per Second for Database Temp Space<br/>' >> index.html
echo '		<img src="./temp.blkwrtnps.png" />' >> index.html
echo '' >> index.html
echo '		<hr/>' >> index.html
echo '' >> index.html
echo '	</body>' >> index.html
echo '</html>' >> index.html
