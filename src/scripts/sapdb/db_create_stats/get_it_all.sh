# Get table and database statistics from TABLESTATISTICS
# To run:  bash get_it_all.sh 

/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm db_warm

cd queries
echo "Getting table statistics"
date
bash doit.sh
cd ../info
echo "Pulling data from statistics"
echo "Table information"
bash tableinfo.sh
echo "Sizing information"
for INFO in       all   index   leaf
do
bash space_used_$INFO.sh
done

echo "Finished getting table statistics"
date
