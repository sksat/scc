#!/bin/bash

try(){
	expected=$1
	input=$2
	./scc "$input" > tmp.s
	if [ $? != 0 ]; then
		echo "test failed."
		echo "input: $input"
		exit -1
	fi
	clang -m32 tmp.s -o tmp.bin
	./tmp.bin
	ret=$?
	if [ $ret != $expected ]; then
		echo "expected $expected, but got $ret"
		exit -1
	fi
	echo "ok[$ret = $input]"
	rm tmp.s tmp.bin
}

try 0 0
try 1 1
try 2 2
try 3 1+2
try 6 1+2+3
try 2 1+2+3-4
try 2 "1 + 2  + 3 - 4"
