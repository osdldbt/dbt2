# Set stats for all of these tables:
#for TABLE in STOCK ITEM HISTORY
for TABLE in CUSTOMER NEW_ORDER ORDERS ORDER_LINE WAREHOUSE DISTRICT STOCK ITEM HISTORY
do
bash ../runsql.sh q$TABLE
cp outfile ../info/stats_$TABLE

done
