#!/usr/bin/perl -w

# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 4 August 2003

use strict;
use Getopt::Long;
use Statistics::Descriptive;

my $mix_log;

GetOptions(
	"if=s" => \$mix_log
);

print "analyzing $mix_log\n";

# Open a file handle to mix.log.
open(FH, "< $mix_log")
	or die "Couldn't open $mix_log for reading: $!\n";

# Load mix.log into memory.  Hope perl doesn't choke...
my $line;
my %data;
my %last_time;
my %error_count;
my $start_time = -1;
my $current_time;
while (defined($line = <FH>) ) {
	chomp $line;
	my @word = split /,/, $line;

	$start_time = $word[ 0 ] if ( $start_time == -1 );
	if ( scalar( @word ) == 4 ) {
		$current_time = $word[ 0 ];
		if ( $word[1] eq 'E' ) {
			++$error_count{ $word[ 3 ] };
		} else {
			++$data{ $word[ 3 ] };
			$last_time{ $word[ 3 ] } = $word[ 0 ];
		}
	}
}

# Do statistics.
my $tid;
my $stat = Statistics::Descriptive::Full->new();

foreach $tid (keys %data) {
	$stat->add_data( $data{ $tid } );
}
my $count = $stat->count();
my $mean = $stat->mean();
my $var  = $stat->variance();
my $stddev = $stat->standard_deviation();
my $median = $stat->median();

# Display the data.
printf( "%9s %4s %12s\n", "---------", "-----", "------------ ------" );
printf( "%9s %4s %12s\n", "Thread ID", "Count", "Last Txn (s) Errors" );
printf( "%9s %4s %12s\n", "---------", "-----", "------------ ------" );
foreach $tid (keys %data) {
	$stat->add_data( $data{ $tid } );
	$error_count{ $tid } = 0 unless ( $error_count{ $tid } );
	$last_time{ $tid } = $current_time + 1 unless ( $last_time{ $tid } );
	printf( "%9d %5d %12d %6d\n", $tid, $data{ $tid },
		$current_time - $last_time{ $tid }, $error_count{ $tid } );
}
print "\n";
print "Statistics:\n";
printf( "run length = %d seconds\n", $current_time - $start_time );
printf( "count = %d\n", $count );
printf( "mean = %4.2f\n", $mean );
printf( "median = %4.2f\n", $median );
printf( "standard deviation = %4.2f\n", $stddev ) if ( $count > 1 );

