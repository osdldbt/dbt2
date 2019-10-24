for TABLE in CUSTOMER NEW_ORDER ORDERS ORDER_LINE WAREHOUSE DISTRICT STOCK ITEM HISTORY

do

STRING_LEVEL=`grep "Index levels" stats_$TABLE | awk -F ";" '{printf"%s",$5}'`
STRING_ROWS=`grep "Rows" stats_$TABLE | awk -F ";" '{printf"%s",$5}'`
STRING_PAGES=`grep "Used  pages" stats_$TABLE | awk -F ";" '{printf"%s",$5}'`

echo  "$TABLE: 		$STRING_LEVEL index levels  	$STRING_ROWS rows 	$STRING_PAGES pages"

done


