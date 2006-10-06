#!/usr/bin/perl -w

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002-2006 Mark Wong & Open Source Development Labs, Inc.
#
# September 2006
# - response time sort to use numeric sort not ascii
# - 90th percentile sort to use numeric sort
# Richard Kennedy EnterpriseDB

use strict;
use Getopt::Long;
use Statistics::Descriptive;
use POSIX qw(ceil floor);

my $mix_log;
my $help;
my $outdir;
my $verbose;

my @delivery_response_time = ();
my @new_order_response_time = ();
my @order_status_response_time = ();
my @payement_response_time = ();
my @stock_level_response_time = ();

my @transactions = ( "delivery", "new_order", "order_status",
	"payment", "stock_level" );
#
# I'm so lazy, and I really don't like perl...
#
my %transaction;
$transaction{ 'd' } = "Delivery";
$transaction{ 'n' } = "New Order";
$transaction{ 'o' } = "Order Status";
$transaction{ 'p' } = "Payment";
$transaction{ 's' } = "Stock Level";
my @xtran = ( "d_tran", "n_tran", "o_tran", "p_tran", "s_tran" );

my $sample_length = 60; # Seconds.

GetOptions(
	"help" => \$help,
	"infile=s" => \$mix_log,
	"outdir=s" => \$outdir,
	"verbose" => \$verbose
);

#
# Because of the way the math works out, and because we want to have 0's for
# the first datapoint, this needs to start at the first $sample_length,
# which is in minutes.
#
my $elapsed_time = 1;

#
# Isn't this bit lame?
#
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

#
# Open a file handle to mix.log.
#
open( FH, "<$mix_log")
	or die "Couldn't open $mix_log for reading: $!\n";

#
# Open a file handle to output data for gnuplot.
#
open( CSV, ">$outdir/notpm.data" )
	or die "Couldn't open $outdir/notpm.data for writing: $!\n";

#
# Load mix.log into memory.  Hope perl doesn't choke...
#
my $line;
my %data;
my %last_time;
my %error_count;
my $errors = 0;

#
# Hashes to determine response time distributions.
#
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

#
# Open separate files because the range of data varies by transaction.
#
open( D_FILE, ">$outdir/d_tran.data" );
open( N_FILE, ">$outdir/n_tran.data" );
open( O_FILE, ">$outdir/o_tran.data" );
open( P_FILE, ">$outdir/p_tran.data" );
open( S_FILE, ">$outdir/s_tran.data" );

my $current_time;
my $start_time;
my $steady_state_start_time = 0;
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

#
# Transaction counts for the steady state portion of the test.
#
$transaction_count{ 'd' } = 0;
$transaction_count{ 'n' } = 0;
$transaction_count{ 'o' } = 0;
$transaction_count{ 'p' } = 0;
$transaction_count{ 's' } = 0;

#
# Read the data directly from the log file and handle it on the fly.
#
print CSV "0 0 0 0 0 0\n";
while ( defined( $line = <FH> ) ) {
	chomp $line;
	my @word = split /,/, $line;

	if (scalar(@word) == 4) {
		#
		# Count transactions per second based on transaction type.
		#
		$current_time = $word[0];
		my $response_time = $word[2];
		#
		# Save the very first start time in the log.
		#
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

			#
			# Reset counters for the next sample interval.
			#
			$current_transaction_count{'d'} = 0;
			$current_transaction_count{'n'} = 0;
			$current_transaction_count{'o'} = 0;
			$current_transaction_count{'p'} = 0;
			$current_transaction_count{'s'} = 0;
		}

		#
		# Determine response time distributions for each transaction
		# type.  Also determine response time for a transaction when
		# it occurs during the run.  Calculate response times for
		# each transaction;
		#
		my $time;
		$time = sprintf("%.2f", $response_time );
		my $x_time = ($word[ 0 ] - $start_time) / 60;
		if ( $word[ 1 ] eq 'd' ) {
			unless ($steady_state_start_time == 0) {
				++$transaction_count{ 'd' };
				$transaction_response_time{ 'd' } += $response_time;
				push @delivery_response_time, $response_time;
				++$current_transaction_count{ 'd' };
			}
			++$d_distribution{ $time };
			print D_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'n' ) {
			unless ($steady_state_start_time == 0) {
				++$transaction_count{ 'n' };
				$transaction_response_time{ 'n' } += $response_time;
				push @new_order_response_time, $response_time;
				++$current_transaction_count{ 'n' };
			}
			++$n_distribution{ $time };
			print N_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'o' ) {
			unless ($steady_state_start_time == 0) {
				++$transaction_count{ 'o' };
				$transaction_response_time{ 'o' } += $response_time;
				push @order_status_response_time, $response_time;
				++$current_transaction_count{ 'o' };
			}
			++$o_distribution{ $time };
			print O_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'p' ) {
			unless ($steady_state_start_time == 0) {
				++$transaction_count{ 'p' };
				$transaction_response_time{ 'p' } += $response_time;
				push @payement_response_time, $response_time;
				++$current_transaction_count{ 'p' };
			}
			++$p_distribution{ $time };
			print P_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 's' ) {
			unless ($steady_state_start_time == 0) {
				++$transaction_count{ 's' };
				$transaction_response_time{ 's' } += $response_time;
				push @stock_level_response_time, $response_time;
				++$current_transaction_count{ 's' };
			}
			++$s_distribution{ $time };
			print S_FILE "$x_time $response_time\n";
		} elsif ( $word[ 1 ] eq 'D' ) {
			++$rollback_count{ 'd' } unless ($steady_state_start_time == 0);
		} elsif ( $word[ 1 ] eq 'N' ) {
			++$rollback_count{ 'n' } unless ($steady_state_start_time == 0);
		} elsif ( $word[ 1 ] eq 'O' ) {
			++$rollback_count{ 'o' } unless ($steady_state_start_time == 0);
		} elsif ( $word[ 1 ] eq 'P' ) {
			++$rollback_count{ 'p' } unless ($steady_state_start_time == 0);
		} elsif ( $word[ 1 ] eq 'S' ) {
			++$rollback_count{ 's' } unless ($steady_state_start_time == 0);
		} elsif ( $word[ 1 ] eq 'E' ) {
			++$errors;
			++$error_count{ $word[ 3 ] };
		}
		
		#
		# Count unknown errors.
		#
		unless ($word[ 1 ] eq 'E' ) {
			++$data{ $word[ 3 ] };
			$last_time{ $word[ 3 ] } = $word[ 0 ];
		}

		$total_response_time += $response_time;
		++$total_transaction_count;
	} elsif (scalar(@word) == 2) {
		#
		# Look for that 'START' marker to determine the end of the rampup time
		# and to calculate the average throughput from that point to the end
		# of the test.
		#
		$steady_state_start_time = $word[0];
	}
}
close( FH );
close( CSV );
close( D_FILE );
close( N_FILE );
close( O_FILE );
close( P_FILE );
close( S_FILE );

#
# Do statistics.
#
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

#
# Display the data.
#
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
foreach my $time ( sort { $a <=> $b } keys %d_distribution  ) {
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
foreach my $time ( sort { $a <=> $b } keys %n_distribution  ) {
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
foreach my $time ( sort { $a <=> $b } keys %o_distribution  ) {
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
foreach my $time ( sort { $a <=> $b } keys %p_distribution  ) {
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
foreach my $time ( sort { $a <=> $b } keys %s_distribution  ) {
	printf( "%8s %5d\n", $time, $s_distribution{ $time } ) if ( $verbose );
	print FILE "$time $s_distribution{ $time }\n"
		if ( $s_distribution{ $time } );
}
close( FILE );
if ( $verbose ) {
	printf( "%8s %5s\n", "--------", "-----" );
}

#
# Create gnuplot input file and generate the charts.
#
chdir $outdir;
my $str = `which gnuplot`;
print "$str\n";
my $has_gnuplot;
if ($str =~ /^no gnuplot/) {
	$has_gnuplot = 0;
} else {
	$has_gnuplot = 1;
}
foreach my $transaction ( @transactions ) {
	last unless ($has_gnuplot);
	my $filename = "$transaction.input";
	open( FILE, ">$filename" )
		or die "cannot open $filename\n";
	print FILE "plot \"$transaction.data\" using 1:2 title \"$transaction\" \n";
	print FILE "set term png small\n";
	print FILE "set output \"$transaction.png\"\n";
	print FILE "set grid xtics ytics\n";
	print FILE "set xlabel \"Response Time (seconds)\"\n";
	print FILE "set ylabel \"Count\"\n";
	print FILE "replot\n";
	close( FILE );
	system "gnuplot $transaction.input";
}

foreach my $transaction ( @xtran ) {
	last unless ($has_gnuplot);
	my $filename = "$transaction" . "-bar.input";
	open( FILE, ">$filename" )
		or die "cannot open $filename\n";
	print FILE "plot \"$transaction.data\" using 1:2 title \"$transaction\" \n";
	print FILE "set term png small\n";
	print FILE "set output \"$transaction" . "_bar.png\"\n";
	print FILE "set grid xtics ytics\n";
	print FILE "set xlabel \"Elapsed Time (Minutes)\"\n";
	print FILE "set ylabel \"Response Time (Seconds)\"\n";
	print FILE "replot\n";
	close( FILE );
	system "gnuplot $filename";
}

#
# Determine 90th percentile response times for each transaction.
#
@delivery_response_time = sort { $a <=> $b } @delivery_response_time;
@new_order_response_time = sort{ $a <=> $b }  @new_order_response_time;
@order_status_response_time = sort { $a <=> $b } @order_status_response_time;
@payement_response_time = sort { $a <=> $b } @payement_response_time;
@stock_level_response_time = sort { $a <=> $b } @stock_level_response_time;
#
# Get the index for the 90th percentile point.
#
my $delivery90index = $transaction_count{'d'} * 0.90;
my $new_order90index = $transaction_count{'n'} * 0.90;
my $order_status90index = $transaction_count{'o'} * 0.90;
my $payment90index = $transaction_count{'p'} * 0.90;
my $stock_level90index = $transaction_count{'s'} * 0.90;

my %response90th;

my $floor;
my $ceil;

$floor = floor($delivery90index);
$ceil = ceil($delivery90index);
if ($floor == $ceil) {
	$response90th{'d'} = $delivery_response_time[$delivery90index];
} else {
	$response90th{'d'} = ($delivery_response_time[$floor] +
			$delivery_response_time[$ceil]) / 2;
}

$floor = floor($new_order90index);
$ceil = ceil($new_order90index);
if ($floor == $ceil) {
	$response90th{'n'} = $new_order_response_time[$new_order90index];
} else {
	$response90th{'n'} = ($new_order_response_time[$floor] +
			$new_order_response_time[$ceil]) / 2;
}

$floor = floor($order_status90index);
$ceil = ceil($order_status90index);
if ($floor == $ceil) {
	$response90th{'o'} = $order_status_response_time[$order_status90index];
} else {
	$response90th{'o'} = ($order_status_response_time[$floor] +
			$order_status_response_time[$ceil]) / 2;
}

$floor = floor($payment90index);
$ceil = ceil($payment90index);
if ($floor == $ceil) {
	$response90th{'p'} = $payement_response_time[$payment90index];
} else {
	$response90th{'p'} = ($payement_response_time[$floor] +
			$payement_response_time[$ceil]) / 2;
}

$floor = floor($stock_level90index);
$ceil = ceil($stock_level90index);
if ($floor == $ceil) {
	$response90th{'s'} = $stock_level_response_time[$stock_level90index];
} else {
	$response90th{'s'} = ($stock_level_response_time[$floor] +
			$stock_level_response_time[$ceil]) / 2;
}

#
# Calculate the actual mix of transactions.
#
printf("                         Response Time (s)\n");
printf(" Transaction      %%    Average :    90th %%        Total        Rollbacks      %%\n");
printf("------------  -----  ---------------------  -----------  ---------------  -----\n");
foreach my $idx ('d', 'n', 'o', 'p', 's') {
	if ($transaction_count{$idx} == 0) {
		printf("%12s   0.00          N/A                      0                0   0.00\n", $transaction{$idx});
	} else {
		printf("%12s  %5.2f  %9.3f : %9.3f  %11d  %15d  %5.2f\n",
				$transaction{$idx},
				($transaction_count{$idx} + $rollback_count{$idx}) /
						$total_transaction_count * 100.0,
				$transaction_response_time{$idx} / $transaction_count{$idx},
				$response90th{$idx},
				$transaction_count{$idx} + $rollback_count{$idx},
				$rollback_count{$idx},
				$rollback_count{$idx} /
						($rollback_count{$idx} + $transaction_count{$idx}) *
						100.0);
	}
}

#
# Calculated the number of transactions per second.
#
my $tps = $transaction_count{'n'} / ($current_time - $start_time);
printf("\n");
printf("%0.2f new-order transactions per minute (NOTPM)\n", $tps * 60);
printf("%0.1f minute duration\n", ($current_time - $start_time) / 60.0);
printf("%d total unknown errors\n", $errors);
printf("%d second(s) ramping up\n", $steady_state_start_time - $start_time);
printf("\n");
