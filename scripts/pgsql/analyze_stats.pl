#!/usr/bin/perl -w

# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2003 Mark Wong & Open Source Development Lab, Inc.
#
# 4 September 2003

use strict;
use Getopt::Long;

my $stats_dir;

GetOptions(
	"if=s" => \$stats_dir
);

#unless ( -d $stats_dir ) {
#	print "$stats_dir directory doesn't exist\n";
#	exit 1;
#}

my @index_names = ( "pk_district", "pk_customer", "pk_new_order", "pk_orders",
	"pk_order_line", "pk_item", "pk_warehouse" );

my @table_names = ( "warehouse", "district", "customer", "history",
	"new_order", "orders", "order_line", "item", "stock" );

my $index;
foreach $index ( @index_names) {
	# Split indexes_scan.out into individual files by index.
	`cat $stats_dir/indexes_scan.out | grep $index | awk '{ print NR, \$11 }' > $stats_dir/$index.index_scan.data`;

	# Recreate the files to be incremental instead of cumulative.
	open( FH, "< $stats_dir/$index.index_scan.data" )
		or die "Couldn't open $index.index_scan.data for reading: $!\n";
	my $line;
	my @data = ();
	while ( defined( $line = <FH> ) ) {
		my @raw_data = split / /, $line;
		push @data, $raw_data[ 1 ];
	}
	close FH;
	`echo "0 0" > $stats_dir/$index.index_scan.data`;
	for ( my $i = 1; $i < scalar( @data ); $i++ ) {
		my $newval = $data[ $i ] - $data[ $i - 1 ];
		`echo "$i $newval" >> $stats_dir/$index.index_scan.data`;
	}

	# Split index_info.out into individual files by index.
	`cat $stats_dir/index_info.out | grep $index | awk '{ print NR, \$9 }' > $stats_dir/$index.index_info.data`;

	# Recreate the files to be incremental instead of cumulative.
	open( FH, "< $stats_dir/$index.index_info.data" )
		or die "Couldn't open $index.index_info.data for reading: $!\n";
	@data = ();
	while ( defined( $line = <FH> ) ) {
		my @raw_data = split / /, $line;
		push @data, $raw_data[ 1 ];
	}
	close FH;
	`echo "0 0" > $stats_dir/$index.index_info.data`;
	for ( my $i = 1; $i < scalar( @data ); $i++ ) {
		my $newval = $data[ $i ] - $data[ $i - 1 ];
		`echo "$i $newval" >> $stats_dir/$index.index_info.data`;
	}
}

my $table;
foreach $table ( @table_names ) {
	# Split table_info.out into individual files by table.
	`cat $stats_dir/table_info.out | grep $table | awk '{ print NR, \$5 }' > $stats_dir/$table.table_info.data`;

	# Recreate the files to be incremental instead of cumulative.
	open( FH, "< $stats_dir/$table.table_info.data" )
		or die "Couldn't open $table.table_info.data for reading: $!\n";
	my $line;
	my @data = ();
	while ( defined( $line = <FH> ) ) {
		my @raw_data = split / /, $line;
		push @data, $raw_data[ 1 ];
	}
	close FH;
	`echo "0 0" > $stats_dir/$table.table_info.data`;
	for ( my $i = 1; $i < scalar( @data ); $i++ ) {
		my $newval = $data[ $i ] - $data[ $i - 1 ];
		`echo "$i $newval" >> $stats_dir/$table.table_info.data`;
	}
}
