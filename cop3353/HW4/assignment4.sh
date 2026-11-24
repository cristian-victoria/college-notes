#Cristian Victoria
#COP3353 - Spring 2023
#Assignment 4 Solution
#Due: Apr 27, 2023


#!/bin/sh

if [ "$#" -ne 1 ]
then
	echo "Usage: assignment4.sh <directory_name>"
	exit 1
elif [ ! -d $1 ]
then
	echo "$1: No such directory"
	exit 2
else
	dir=$1

	dirCount=0
	fileCount=0
	readCount=0
	writeCount=0
	executeCount=0

	for filename in $dir/*
	do
		if [ -d "$filename" ]
		then
			dirCount=`expr $dirCount + 1`
		fi
		if [ -f "$filename" ]
		then
			fileCount=`expr $fileCount + 1`
		fi
	done;

	for filename in $dir/*
	do
		if [ -d "$filename" ]||[ -f "$filename" ]
		then
			if [ -r "$filename" ]
			then
				readCount=`expr $readCount + 1`
			fi
			if [ -w "$filename" ]
                        then
                                writeCount=`expr $writeCount + 1`
                        fi
			if [ -x "$filename" ]
                        then
                                executeCount=`expr $executeCount + 1`
                        fi
		fi
	done;

	echo "Number of directories : $dirCount"
	echo "Number of files : $fileCount"
	echo "Number of readable items : $readCount"
	echo "Number of writable items : $writeCount"
	echo "Number of executable items : $executeCount"
fi

exit
