#!/bin/bash

echo "Expecting rolls in STDIN..."

R=$1
pushd submissions/$R
{
for phase in `seq 1 4`
do
    for part in Client Server
    do
	f="SimpleEmail${part}Phase${phase}"
	rm -f "$f"
	if [ -s "$f.cpp" ]; then
	    rm -f "$f"
	    flags[1]=""
	    flags[2]="-std=c++11"
	    flags[3]="-g -std=c++11"
	    flags[4]="-g -std=c++17"
	    # {
	    # 	echo "#include <stdio.h>"; echo "#include <errno.h>"; cat "$f.cpp"
	    # } > "$f-mod.cpp"
	    for i in 1 2 3 4
	    do
		echo "Trying comping with g++ flags ${flags[$i]}..."
		if [ $i -eq 4 ]; then
			g++ ${flags[$i]} -o "$f" "$f.cpp" -lstdc++fs
		else
			g++ ${flags[$i]} -o "$f" "$f.cpp"
		fi
		
		if [ -x "$f" ]; then
		    break
		fi
	    done
	    if [ -x "$f" ]; then
		echo "$f compilation successful"
	    else
		echo "$f compilation failed"
	    fi
	else
	    msg="File $f.cpp not present or is empty"
	    echo $msg
	    echo $msg 1>&2
	fi
    done
done
} > g++-compile-out.txt 2>g++-compile-out.err
popd
