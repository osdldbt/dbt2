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
my $verbose;

my @transactions = ( "delivery", "new_order", "order_status",
	"payment", "stock_level" );
# I'm so lazy, and I really don't like perl...
my %transactions;
$transactions{ 'd' } = "Delivery";
$transactions{ 'n' } = "New Order";
$transactions{ 'o' } = "Order Status";
$transactions{ 'p' } = "Payment";
$transactions{ 's' } = "Stock Level";
my @xtran = ( "d_tran", "n_tran", "o_tran", "p_tran", "s_tran" );

my $sample_length = 60; # Seconds.

GetOptions(
	"help" => \$help,
	"infile=s" => \$mix_log,
	"outdir=s" => \$outdir,
	"verbose" => \$verbose
);

# Because of the way the math works out, and because we want to have 0's for
# the first datapoint, this needs to start at the first $sample_length,
# which is in minutes.
my $elapsed_time = 1;

# Isn't this bit lame?
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
open( FH, "<$mix_log")
	or die "Couldn't open $mix_log for reading: $!\n";
open( CSV, ">$outdir/notpm.data" )
	or die "Couldn't open $outdir/notpm.data for writing: $!\n";

# Load mix.log into memory.  Hope perl doesn't choke...
my $line;
my %data;
my %last_time;
my %error_count;
my $errors = 0;

# Hashes to determine response time distributions.
my %d_distribution;
my %n_distribution;
my %o_distribution;
my %p_distribution;
my %s_distribution;

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

# Open separate files because the range of data varies by transaction.
open( D_FILE, ">$outdir/d_tran.data" );
open( N_FILE, ">$outdir/n_tran.data" );
open( O_FILE, ">$outdir/o_tran.data" );
open( P_FILE, ">$outdir/p_tran.data" );
open( S_FILE, ">$outdir/s_tran.data" );

my $current_time;
my $start_time;
my $previous_time;
my $total_response_time;
my $total_transaction_count;
my $response_time;

my %current_transaction_count;
my %rollback_count;
my %transaction_count;
my %transaction_response_time;

$current_transaction_count{ 'd' } = 0;
$current_transaction_count{ 'n' } = 0;
$current_transaction_count{ 'o' } = 0;
$current_transaction_count{ 'p' } = 0;
$current_transaction_count{ 's' } = 0;

$rollback_count{ 'd' } = 0;
$rollback_count{ 'n' } = 0;
$rollback_count{ 'o' } = 0;
$rollback_count{ 'p' } = 0;
$rollback_count{ 's' } = 0;

$transaction_count{ 'd' } = 0;
$transaction_count{ 'n' } = 0;
$transaction_count{ 'o' } = 0;
$transaction_count{ 'p' } = 0;
$transaction_count{ 's' } = 0;

# Collect stats.
print CSV "0 0 0 0 0 0\n";
while ( defined( $line = <FH> ) ) {
	chomp $line;
	my @word = split /,/, $line;

	if ( scalar( @word ) == 4 ) {
		# Count transactions per second based on transaction type.
		$current_time = $word[ 0 ];
		my $response_time = $word[ 2 ];
		unless ( $start_time ) {
			$start_time = $previous_time = $current_time;
		}
		if ( $current_time >= ( $previous_time + $sample_length ) ) {
			print CSV "$elapsed_time "
				. "$current_transaction_count{ 'd' } "
				. "$current_transaction_count{ 'n' } "
				. "$current_transaction_count{ 'o' } "
				. "$current_transaction_count{ 'p' } "
				. "$current_transaction_count{ 's' }\n";

			++$elapsed_time;
			$previous_time = $current_time;

			# Reset counters for the next sample interval.
			$current_transaction_count{ 'd' } = 0;
			$current_transaction_count{ 'n' } = 0;
			$current_transaction_count{ 'o' } = 0;
			$current_transaction_count{ 'p' } = 0;
			$current_transaction_count{ 's' } = 0;
		}

		# Determine response time distributions for each transaction
		# type.  Also determine response time for a transaction when
		# it occurs during the run.  Calculate response times for
		# each transaction;
		my $time;
		$time = sprintf("%.2f", $response_time );
		my $x_time = ($word[ 0 ] - $start_time) / 60;
		if ( $word[ 1 ] eq 'd' ) {
			++$transaction_count{ 'd' };
			$transaction_response_time{ 'd' } += $response_time;
			++$current_transaction_count{ 'd' };

			++$d_distribution{ $time };
			print D_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'n' ) {
			++$transaction_count{ 'n' };
			$transaction_response_time{ 'n' } += $response_time;
			++$current_transaction_count{ 'n' };

			++$n_distribution{ $time };
			print N_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'o' ) {
			++$transaction_count{ 'o' };
			$transaction_response_time{ 'o' } += $response_time;
			++$current_transaction_count{ 'o' };

			++$o_distribution{ $time };
			print O_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'p' ) {
			++$transaction_count{ 'p' };
			$transaction_response_time{ 'p' } += $response_time;
			++$current_transaction_count{ 'p' };

			++$p_distribution{ $time };
			print P_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 's' ) {
			++$transaction_count{ 's' };
			$transaction_response_time{ 's' } += $response_time;
			++$current_transaction_count{ 's' };

			++$s_distribution{ $time };
			print S_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'D' ) {
			++$rollback_count{ 'd' };
			++$transaction_count{ 'd' };
			$transaction_response_time{ 'd' } += $response_time;
			++$current_transaction_count{ 'd' };
		} elsif ( $word[ 1 ] eq 'N' ) {
			++$rollback_count{ 'n' };
			++$transaction_count{ 'n' };
			$transaction_response_time{ 'n' } += $response_time;
			++$current_transaction_count{ 'n' };
		} elsif ( $word[ 1 ] eq 'O' ) {
			++$rollback_count{ 'o' };
			++$transaction_count{ 'o' };
			$transaction_response_time{ 'o' } += $response_time;
			++$current_transaction_count{ 'o' };
		} elsif ( $word[ 1 ] eq 'P' ) {
			++$rollback_count{ 'p' };
			++$transaction_count{ 'p' };
			$transaction_response_time{ 'p' } += $response_time;
			++$current_transaction_count{ 'p' };
		} elsif ( $word[ 1 ] eq 'S' ) {
			++$rollback_count{ 's' };
			++$transaction_count{ 's' };
			$transaction_response_time{ 's' } += $response_time;
			++$current_transaction_count{ 's' };
		} elsif ( $word[ 1 ] eq 'E' ) {
			++$errors;
			++$error_count{ $word[ 3 ] };
		}
		
		unless ($word[ 1 ] eq 'E' ) {
			++$data{ $word[ 3 ] };
			$last_time{ $word[ 3 ] } = $word[ 0 ];
		}

		$total_response_time += $response_time;
		++$total_transaction_count;
	}
}
close( FH );
close( CSV );
close( D_FILE );
close( N_FILE );
close( O_FILE );
close( P_FILE );
close( S_FILE );

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
if ( $verbose ) {
	printf( "%10s %4s %12s\n", "----------", "-----",
		"------------ ------" );
	printf( "%10s %4s %12s\n", "Thread ID", "Count",
		"Last Txn (s) Errors" );
	printf( "%10s %4s %12s\n", "----------", "-----",
		"------------ ------" );
}
foreach $tid ( keys %data ) {
	$stat->add_data( $data{ $tid } );
	$error_count{ $tid } = 0 unless ( $error_count{ $tid } );
	$last_time{ $tid } = $current_time + 1 unless ( $last_time{ $tid } );
	printf( "%9d %5d %12d %6d\n", $tid, $data{ $tid },
		$current_time - $last_time{ $tid }, $error_count{ $tid } )
		if ( $verbose );
}
if ( $verbose ) {
	printf( "%10s %4s %12s\n", "----------", "-----",
		"------------ ------" );
	print "\n";
	print "Statistics Over All Transactions:\n";
	printf( "run length = %d seconds\n", $current_time - $start_time );
	printf( "count = %d\n", $count );
	printf( "mean = %4.2f\n", $mean );
	printf( "min = %4.2f\n", $min );
	printf( "max = %4.2f\n", $max );
	printf( "median = %4.2f\n", $median );
	printf( "standard deviation = %4.2f\n", $stddev ) if ( $count > 1 );

	print "\n";
}

if ( $verbose ) {
	print "Delivery Response Time Distribution\n";
	printf( "%8s %5s\n", "--------", "-----" );
	printf( "%8s %5s\n", "Time (s)", "Count" );
	printf( "%8s %5s\n", "--------", "-----" );
}
open( FILE, ">$outdir/delivery.data" );
foreach my $time ( sort keys %d_distribution  ) {
	printf( "%8s %5d\n", $time, $d_distribution{ $time } ) if ( $verbose );
	print FILE "$time $d_distribution{ $time }\n"
		if ( $d_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
	print "\n";

	print "New Order Response Time Distribution\n";
	printf( "%8s %5s\n", "--------", "-----" );
	printf( "%8s %5s\n", "Time (s)", "Count" );
	printf( "%8s %5s\n", "--------", "-----" );
}
open( FILE, ">$outdir/new_order.data" );
foreach my $time ( sort keys %n_distribution  ) {
	printf( "%8s %5d\n", $time, $n_distribution{ $time } ) if ( $verbose );
	print FILE "$time $n_distribution{ $time }\n"
		if ( $n_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
	print "\n";

	print "Order Status Response Time Distribution\n";
	printf( "%8s %5s\n", "--------", "-----" );
	printf( "%8s %5s\n", "Time (s)", "Count" );
	printf( "%8s %5s\n", "--------", "-----" );
}
open( FILE, ">$outdir/order_status.data" );
foreach my $time ( sort keys %o_distribution  ) {
	printf( "%8s %5d\n", $time, $o_distribution{ $time } ) if ( $verbose );
	print FILE "$time $o_distribution{ $time }\n"
		if ( $o_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
	print "\n";

	print "Payment Response Time Distribution\n";
	printf( "%8s %5s\n", "--------", "-----" );
	printf( "%8s %5s\n", "Time (s)", "Count" );
	printf( "%8s %5s\n", "--------", "-----" );
}
open( FILE, ">$outdir/payment.data" );
foreach my $time ( sort keys %p_distribution  ) {
	printf( "%8s %5d\n", $time, $p_distribution{ $time } ) if ( $verbose );
	print FILE "$time $p_distribution{ $time }\n"
		if ( $p_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
	print "\n";

	print "Stock Level Response Time Distribution\n";
	printf( "%8s %5s\n", "--------", "-----" );
	printf( "%8s %5s\n", "Time (s)", "Count" );
	printf( "%8s %5s\n", "--------", "-----" );
}
open( FILE, ">$outdir/stock_level.data" );
foreach my $time ( sort keys %s_distribution  ) {
	printf( "%8s %5d\n", $time, $s_distribution{ $time } ) if ( $verbose );
	print FILE "$time $s_distribution{ $time }\n"
		if ( $s_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
}

# Create gnuplot input file and generate the charts.
chdir $outdir;
foreach my $transaction ( @transactions ) {
	my $filename = "$transaction.input";
	open( FILE, ">$filename" )
		or die "cannot open $filename\n";
	print FILE "plot \"$transaction.data\" using 1:2 title \"$transaction\" \n";
	print FILE "set term png small\n";
	print FILE "set output \"$transaction.png\"\n";
	print FILE "set xlabel \"Response Time (seconds)\"\n";
	print FILE "set ylabel \"Count\"\n";
	print FILE "replot\n";
	close( FILE );
	system "gnuplot -noraise $transaction.input";
}

foreach my $transaction ( @xtran ) {
	my $filename = "$transaction" . "-bar.input";
	open( FILE, ">$filename" )
		or die "cannot open $filename\n";
	print FILE "plot \"$transaction.data\" using 1:2 title \"$transaction\" \n";
	print FILE "set term png small\n";
	print FILE "set output \"$transaction" . "_bar.png\"\n";
	print FILE "set xlabel \"Elapsed Time (Minutes)\"\n";
	print FILE "set ylabel \"Response Time (Seconds)\"\n";
	print FILE "replot\n";
	close( FILE );
	system "gnuplot -noraise $filename";
}

# Calculate the actual mix of transactions.
printf( " Transaction      %%  Avg Response Time (s)        Total        Rollbacks      %%\n" );
printf( "------------  -----  ---------------------  -----------  ---------------  -----\n" );
foreach my $idx ( 'd', 'n', 'o', 'p', 's' ) {
	printf("%12s  %5.2f  %21.3f  %11d  %15d  %5.2f\n",
		$transactions{ $idx }, $transaction_count{ $idx } /
		$total_transaction_count * 100.0,
		$transaction_response_time{ $idx } /
		$transaction_count{ $idx },
		$transaction_count{ $idx }, $rollback_count{ $idx },
		$rollback_count{ $idx } /
		$transaction_count{ $idx } * 100.0);
}

# Calculated the number of transactions per second.
my $tps = $transaction_count{ 'n' } / ($current_time - $start_time);
printf("\n");
printf("%0.2f new-order transactions per minute (NOTPM)\n", $tps * 60);
printf("%0.1f minute duration\n", ($current_time - $start_time) / 60.0);
printf("%d total unknown errors\n", $errors);
printf("\n");
