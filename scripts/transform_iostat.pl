#!/usr/bin/perl -w

# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2003 Mark Wong & Open Source Development Lab, Inc.
#
# 3 September 2003

use strict;
use Getopt::Long;

my $iostat_data;

GetOptions(
	"if=s" => \$iostat_data
);

unless ( $iostat_data ) {
	print "usage: transform_iostat.pl --if <iostat.out>\n";
	exit 1;
}

unless ( -f $iostat_data ) {
	print "$iostat_data doesn't exist\n";
	exit 1;
}

my $iostatcsv = $iostat_data . ".csv";
`cat $iostat_data | grep -v '^Device' | grep -v '^\$' | awk '{ print \$1\",\"\$2",\"\$3\",\"\$4\",\"\$5\",\"\$6 }' > $iostatcsv`;

open( FH, "< $iostatcsv" )
	or die "Couldn't open $iostatcsv for reading: $!\n";

my $line;
my %device;

while ( defined( $line = <FH> ) ) {
	my @raw_data = split /,/, $line;

	push @{$device{ $raw_data[ 0 ] }{ tps } }, $raw_data[ 1 ];
	push @{$device{ $raw_data[ 0 ] }{ blkreadps } }, $raw_data[ 2 ];
	push @{$device{ $raw_data[ 0 ] }{ blkwrtnps } }, $raw_data[ 3 ];
	push @{$device{ $raw_data[ 0 ] }{ blkread } }, $raw_data[ 4 ];
	push @{$device{ $raw_data[ 0 ] }{ blkwrtn } }, $raw_data[ 5 ];
}

for my $devname ( keys %device ) {
	my $count = scalar( @{$device{ $devname }{ tps } } );

	# Start at 1 because 0 appears to have bogus data.
	for ( my $i = 1; $i < $count; $i++ ) {
		system "echo \"$i ${$device{ $devname }{ tps } }[ $i ]\" >> $iostat_data" . "." . $devname . ".tps.data";
		system "echo \"$i ${$device{ $devname }{ blkreadps } }[ $i ]\" >> $iostat_data" . "." . $devname . ".blkreadps.data";
		system "echo \"$i ${$device{ $devname }{ blkwrtnps } }[ $i ]\" >> $iostat_data" . "." . $devname . ".blkwrtnps.data";
		system "echo \"$i ${$device{ $devname }{ blkread } }[ $i ]\" >> $iostat_data" . "." . $devname . ".blkread.data";
		system "echo \"$i ${$device{ $devname }{ blkwrtn } }[ $i ]\" >> $iostat_data" . "." . $devname . ".blkwrtn.data";

	}
}
