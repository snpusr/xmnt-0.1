#!/bin/bash
# helper script
# 15/06/2016
# -snp
#
# Tue Oct 25 09:27:43 BRST 2016
# added '-cpu core2duo' to qemu argument for bypassing VM detection on iVirtua 11
#
#

# hardcoded into asm stub and fuse driver
PORT="4555"
GDBPORT="1234"

# script used to patch and bypass protections
SCRIPT="inject-new.py"
PAYLOAD='asm/main'
QEMUOPTS="-nographic -m 1024 -cpu core2duo -s -S -serial tcp:127.0.0.1:$PORT,server,nowait,nodelay -drive cache=off,format=raw,file="

function check_needed()
{
	local ex=0

	[ "$GDB" == "" ] && GDB=''`which ggdb`'' 

	if [ "$GDB" == "" ] 
	then 
		echo "GDB not found";
		ex=1
	fi
	if [ "$QEMU" == "" ]
	then
		echo "QEMU not found";
		ex=1
	fi
	return $ex
}
function goodbye()
{
	echo "exiting"
	exit -1
}
function check_params()
{
	if [ "$1" == "" ] 
	then
		echo "usage: $0 [device or disk image]";
		return 1;
	fi
	DISK_IMAGE=''$1''

	return 0
}

GDB=`which gdb`
QEMU=`which qemu-system-i386`
DISK_IMAGE=''

check_params $* || goodbye
if [ -f .gdbpid ] && [ -f .qemupid ] 
then
	echo "umount crypto/ first";
	exit;
fi
check_needed || goodbye

$QEMU $QEMUOPTS"$DISK_IMAGE" >/dev/null  &
qemupid=$!
$GDB -q --nh --command $SCRIPT --batch &
gdbpid=$!

echo $gdbpid > .gdbpid
echo $qemupid > .qemupid
echo "waiting stub to execute..."
sleep 4
echo "mounting disk..., please wait...";
time -p (./bin/lowntfs-3g crypto: crypto/) 2> .time
secs=`grep real .time | cut -d' ' -f2`
echo "Done. $1 mounted on crypto/ directory in $secs seconds"
rm .time*
