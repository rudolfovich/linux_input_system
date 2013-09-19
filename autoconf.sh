#!/bin/sh

function run_checked
{
	echo "Running [$@]..."
	$@
	if [ 0 -ne $? ]
	then
		echo "[$@] failed with code $?!" 
		exit 1
	else
		echo "  ok"
	fi
}

echo "Looking for file configure.ac..."
if [ ! -f configure.ac ]
then
	echo "  file not found!"
	
	run_checked autoscan
	mv configure.scan configure.ac
	#TODO: sed ...
	#AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
	run_checked vim configure.ac
else # if [ ! -f configure.ac ]
	echo "  file exists!"
fi # if [ ! -f configure.ac ]

run_checked autoheader
run_checked aclocal
run_checked automake --add-missing
run_checked autoconf

