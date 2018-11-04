#!/bin/bash

try(){
	expected=$1
	input=$2
	echo "$input" > tmp.c
	./scc tmp.c > tmp.s
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
	rm tmp.c tmp.s tmp.bin
}

try_dir(){
	for src in `find test -maxdepth 1 -type f`; do
		echo $src
		./scc $src > tmp.s
		if [ $? != 0 ]; then
			echo "test failed."
			exit -1
		fi
		clang -m32 tmp.s -o tmp.bin
		./tmp.bin
		ret=$?
		echo "ret($ret)"
		rm tmp.s tmp.bin
	done
}

try 0 0
try 1 1
try 2 2
try 3 1+2
try 6 1+2+3
try 2 1+2+3-4
try 2 "1 + 2  + 3 - 4"
try 2 1*2
try 7 1+2*3
try 2 4/2
try 10 1+2*3+6/2
try 3 "(1+2)"
try 15 "1+2*(3+4)"
try 114 "2*(3+(4+5)*6)"
try_dir
