#!/bin/sh

# run_test.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 18 october 2002

if [ $# -lt 6 ]; then
    echo "usage: run_test.sh --duration <sec> --sample <sec> -w <warehouses>"
    exit
fi

while :
do
	case $# in
	0)
		break
		;;
	esac

	option=$1
	shift

	orig_option=$option
	case $option in
	--*)
		;;
	-*)
		option=-$option
		;;
	esac

	case $option in
	--*=*)
		optarg=`echo $option | sed -e 's/^[^=]*=//'`
		arguments="$arguments $option"
		;;
	--sample | --duration | --wmin | --wmax | -w | --ktd | --ktn | --kto | --ktp | --kts | --ttd | --ttn | --tto | --ttp | --tts | --tpw)
		optarg=$1
		shift
		arguments="$arguments $option=$optarg"
		;;
	esac

	case $option in
	--duration)
		DURATION=$optarg
		;;
	--sample)
		SAMPLE_LENGTH=$optarg
		;;
	--wmin)
		WMIN=$optarg
		;;
	--wmax)
		WMAX=$optarg
		;;
	--w)
		W=$optarg
		;;
	--ktd)
		KTD=$optarg
		;;
	--ktn)
		KTN=$optarg
		;;
	--kto)
		KTO=$optarg
		;;
	--ktp)
		KTP=$optarg
		;;
	--kts)
		KTS=$optarg
		;;
	--ttd)
		TTD=$optarg
		;;
	--ttn)
		TTN=$optarg
		;;
	--tto)
		TTO=$optarg
		;;
	--ttp)
		TTP=$optarg
		;;
	--tts)
		TTS=$optarg
		;;
	--tpw)
		TPW=$optarg
		;;
	esac
done

ITERATION=`expr $DURATION / $SAMPLE_LENGTH`

# determine run number for selecting an output directory
RUN_NUMBER=-1
read RUN_NUMBER < .run_number
if [ $RUN_NUMBER -eq -1 ]; then
	RUN_NUMBER=0
fi
mkdir -p output
mkdir -p output/$RUN_NUMBER
OUTPUT_DIR=output/$RUN_NUMBER
RUN_NUMBER=`expr $RUN_NUMBER + 1`
echo $RUN_NUMBER > .run_number

# Capture a comment.
echo "Enter a short description about this run:"
read COMMENT
echo "$COMMENT" >> $OUTPUT_DIR/readme.txt

# start the database
sapdb/start_db.sh

cd $OUTPUT_DIR
../../sapdb/plan.sh > plan0.out
cd -

# start system stat gathering
./sysstats.sh --db sapdb --dbname dbt2 --outdir $OUTPUT_DIR --iter $ITERATION --sample $SAMPLE_LENGTH &

# start a test run
CMD="../terminal/driver -dbname dbt2 -w $W -l $DURATION"

# generate the command line argument for the driver depending on the
# flags used
if [ -z $WMIN ]; then
	WMIN=1
fi
CMD="$CMD -wmin $WMIN"

if [ -z $WMAX ]; then
	WMAX=1
fi
CMD="$CMD -wmax $WMAX"

if ! [ -z $KTD ]; then
	CMD="$CMD -ktd $KTD"
fi

if ! [ -z $KTN ]; then
	CMD="$CMD -ktn $KTN"
fi

if ! [ -z $KTO ]; then
	CMD="$CMD -kto $KTO"
fi

if ! [ -z $KTP ]; then
	CMD="$CMD -ktp $KTP"
fi

if ! [ -z $KTS ]; then
	CMD="$CMD -kts $KTS"
fi

if ! [ -z $TTD ]; then
	CMD="$CMD -ttd $TTD"
fi

if ! [ -z $TTN ]; then
	CMD="$CMD -ttn $TTN"
fi

if ! [ -z $TTO ]; then
	CMD="$CMD -tto $TTO"
fi

if ! [ -z $TTP ]; then
	CMD="$CMD -ttp $TTP"
fi

if ! [ -z $TTS ]; then
	CMD="$CMD -tts $TTS"
fi

if ! [ -z $TPW ]; then
	CMD="$CMD -tpw $TPW"
fi

# start oprofile 
sudo ./oprof.sh /vmlinux 300000 $DURATION oprofile $OUTPUT_DIR &

# start the driver
$CMD

# copy the terminal output files to the output directory
echo "moving mix.log and error.log"
mv mix.log $OUTPUT_DIR/
mv error.log $OUTPUT_DIR/

# analyze the results of mix.log
../tools/results $OUTPUT_DIR/mix.log $SAMPLE_LENGTH $OUTPUT_DIR

echo "run complete"

# stop the database
sapdb/stop_db.sh

# draw graphs
cd $OUTPUT_DIR
gnuplot ../../bt.input
