#!/bin/sh

if [ $# -ne 5 ]; then
	echo "usage: prof.sh <vmlinux> <ctr0-count> <sleep> <session> <outdir>"
	exit
fi

export PATH=/sbin/:/usr/sbin:$PATH

# stop just in case a previous session is running and move all the current
# data into a place we don't care about
op_stop
op_session --session=trash
rm -rf /var/lib/oprofile/samples/trash

# start a new session and save it
op_start --vmlinux=$1 --ctr0-event=CPU_CLK_UNHALTED --ctr0-count=$2
sleep $3
op_stop
op_session --session=$4

chown -R sapdb /var/lib/oprofile/samples/oprofile
mv /var/lib/oprofile/samples/oprofile $5
