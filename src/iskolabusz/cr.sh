#!/usr/bin/env bash

/usr/bin/g++-6 --version
for file in /mnt/stuff/a*.cpp; do
    o="${file%.*}"
    g++-6 "$file" -o "$o"
    if [ $? -ne 0 ]; then
        continue
    fi
    echo -ne "Executing $o: "
    for i in `seq 1 10`; do
        r=$(echo "$i" | $o)
        echo -ne "$r "
    done
    echo ""
done
