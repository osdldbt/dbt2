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
my $help;
my $outdir;

my @transactions = ( "delivery", "new_order", "order_status",
	"payment", "stock_level" );

GetOptions(
	"help" => \$help,
	"infile=s" => \$mix_log,
	"outdir=s" => \$outdir
);

if ( $help ) {
	print "usage: mix_analyzer.pl --infile mix.log --outdir <path>\n";
	exit 1;
}

unless ( $mix_log ) {
	print "usage: mix_analyzer.pl --infile mix.log --outdir <path>\n";
	exit 1;
}

unless ( $outdir ) {
	print "usage: mix_analyzer.pl --infile mix.log --outdir <path>\n";
	exit 1;
}

# Open a file handle to mix.log.
open( FH, "< $mix_log")
	or die "Couldn't open $mix_log for reading: $!\n";
open( NEWMIX, ">$outdir/newmix.log" );

# Load mix.log into memory.  Hope perl doesn't choke...
my $line;
my %data;
my %last_time;
my %error_count;
my $start_time = -1;
my $current_time;
my %d_distribution;
my %n_distribution;
my %o_distribution;
my %p_distribution;
my %s_distribution;
my $first;
my %transaction_name;
$transaction_name{ "d" } = "delivery";
$transaction_name{ "n" } = "new order";
$transaction_name{ "o" } = "order status";
$transaction_name{ "p" } = "payment";
$transaction_name{ "s" } = "stock level";
$transaction_name{ "D" } = "delivery";
$transaction_name{ "N" } = "new order";
$transaction_name{ "O" } = "order status";
$transaction_name{ "P" } = "payment";
$transaction_name{ "S" } = "stock level";
$transaction_name{ "E" } = "unknown error";
while ( defined( $line = <FH> ) ) {
	chomp $line;
	my @word = split /,/, $line;

	$start_time = $word[ 0 ] if ( $start_time == -1 );
	if ( scalar( @word ) == 4 ) {
		# Reformat a new mix.log file.
		$first = $word[ 0 ] - $word[ 2 ] unless ( $first );
		printf( NEWMIX "%f	%s	%f\n",
			$word[0] - ( $first + $word[ 2 ] ),
			$transaction_name{ $word[ 1 ] }, $word[ 2 ] );

		# Collect stats.
		# Count the number of transactions per thread.
		$current_time = $word[ 0 ];
		if ( $word[ 1 ] eq 'E' ) {
			++$error_count{ $word[ 3 ] };
		} else {
			++$data{ $word[ 3 ] };
			$last_time{ $word[ 3 ] } = $word[ 0 ];
		}

		# Determine response time distributions for each transaction
		# type.
		my $time;
		$time = sprintf("%.2f", $word[ 2 ] );
		if ( $word[ 1 ] eq 'd' ) {
			++$d_distribution{ $time };
		} elsif ( $word[ 1 ] eq 'n' ) {
			++$n_distribution{ $time };
		} elsif ( $word[ 1 ] eq 'o' ) {
			++$o_distribution{ $time };
		} elsif ( $word[ 1 ] eq 'p' ) {
			++$p_distribution{ $time };
		} elsif ( $word[ 1 ] eq 's' ) {
			++$s_distribution{ $time };
		}
	}
}
close( FH );
close( NEWMIX );

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
my $min = $stat->min();
my $max = $stat->max();

# Display the data.
printf( "%10s %4s %12s\n", "----------", "-----", "------------ ------" );
printf( "%10s %4s %12s\n", "Thread ID", "Count", "Last Txn (s) Errors" );
printf( "%10s %4s %12s\n", "----------", "-----", "------------ ------" );
foreach $tid ( keys %data ) {
	$stat->add_data( $data{ $tid } );
	$error_count{ $tid } = 0 unless ( $error_count{ $tid } );
	$last_time{ $tid } = $current_time + 1 unless ( $last_time{ $tid } );
	printf( "%9d %5d %12d %6d\n", $tid, $data{ $tid },
		$current_time - $last_time{ $tid }, $error_count{ $tid } );
}
printf( "%10s %4s %12s\n", "----------", "-----", "------------ ------" );
print "\n";
print "Statistics:\n";
printf( "run length = %d seconds\n", $current_time - $start_time );
printf( "count = %d\n", $count );
printf( "mean = %4.2f\n", $mean );
printf( "min = %4.2f\n", $min );
printf( "max = %4.2f\n", $max );
printf( "median = %4.2f\n", $median );
printf( "standard deviation = %4.2f\n", $stddev ) if ( $count > 1 );

print "\n";

print "Delivery Response Time Distribution\n";
printf( "%8s %5s\n", "--------", "-----" );
printf( "%8s %5s\n", "Time (s)", "Count" );
printf( "%8s %5s\n", "--------", "-----" );
open( FILE, ">$outdir/delivery.data" );
foreach my $time ( sort keys %d_distribution  ) {
	printf( "%8s %5d\n", $time, $d_distribution{ $time } );
	print FILE "$time $d_distribution{ $time }\n";
}
close( FILE );
printf( "%8s %5s\n", "--------", "-----" );
print "\n";

print "New Order Response Time Distribution\n";
printf( "%8s %5s\n", "--------", "-----" );
printf( "%8s %5s\n", "Time (s)", "Count" );
printf( "%8s %5s\n", "--------", "-----" );
open( FILE, ">$outdir/new_order.data" );
foreach my $time ( sort keys %n_distribution  ) {
	printf( "%8s %5d\n", $time, $n_distribution{ $time } );
	print FILE "$time $d_distribution{ $time }\n";
}
close( FILE );
printf( "%8s %5s\n", "--------", "-----" );
print "\n";

print "Order Status Response Time Distribution\n";
printf( "%8s %5s\n", "--------", "-----" );
printf( "%8s %5s\n", "Time (s)", "Count" );
printf( "%8s %5s\n", "--------", "-----" );
open( FILE, ">$outdir/order_status.data" );
foreach my $time ( sort keys %o_distribution  ) {
	printf( "%8s %5d\n", $time, $o_distribution{ $time } );
	print FILE "$time $d_distribution{ $time }\n";
}
close( FILE );
printf( "%8s %5s\n", "--------", "-----" );
print "\n";

print "Payment Response Time Distribution\n";
printf( "%8s %5s\n", "--------", "-----" );
printf( "%8s %5s\n", "Time (s)", "Count" );
printf( "%8s %5s\n", "--------", "-----" );
open( FILE, ">$outdir/payment.data" );
foreach my $time ( sort keys %p_distribution  ) {
	printf( "%8s %5d\n", $time, $p_distribution{ $time } );
	print FILE "$time $d_distribution{ $time }\n";
}
close( FILE );
printf( "%8s %5s\n", "--------", "-----" );
print "\n";

print "Stock Level Response Time Distribution\n";
printf( "%8s %5s\n", "--------", "-----" );
printf( "%8s %5s\n", "Time (s)", "Count" );
printf( "%8s %5s\n", "--------", "-----" );
open( FILE, ">$outdir/stock_level.data" );
foreach my $time ( sort keys %s_distribution  ) {
	printf( "%8s %5d\n", $time, $s_distribution{ $time } );
	print FILE "$time $d_distribution{ $time }\n";
}
close( FILE );
printf( "%8s %5s\n", "--------", "-----" );

# Create gnuplot input file and generate the charts.
chdir $outdir;
foreach my $transaction ( @transactions ) {
	open( FILE, ">$outdir/$transaction.input" );
	print FILE "plot \"$transaction.data\" using 1:2 title \"$transaction\" \n";
	print FILE "set term png small\n";
	print FILE "set output \"$transaction.png\"\n";
	print FILE "set xlabel \"Response Time (seconds)\"\n";
	print FILE "set ylabel \"Count\"\n";
	print FILE "replot\n";
	close( FILE );
	system "gnuplot $transaction.input";
}

