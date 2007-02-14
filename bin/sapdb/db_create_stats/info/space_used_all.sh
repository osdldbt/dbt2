STRING_PAGES=`grep "Used  pages" stat* | awk -F ";" '{printf"%s+",$5}'`
echo ${STRING_PAGES}0 > bc_input
PAGES=`bc < bc_input`
(( KBYTES = $PAGES * 8 ))
(( MBYTES = $KBYTES / 1024 ))
(( GBYTES = $MBYTES / 1024 ))

echo " Size Used for all tables:  "
echo "   PAGES $PAGES (8k bytes)"
echo "   KB    $KBYTES"
echo "   MB    $MBYTES"
echo "   GB    $GBYTES"




