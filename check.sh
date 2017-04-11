#!/usr/bin/env bash

if [ $# -ne 3 ]; then
    echo "Usage: $0 build-dir first-ai-file second-ai-file"
    echo "measures whether second is a better ai than first"
    exit 1
fi

#generator="./build-clang_release/src/lake/generator"
build_dir="$1"
generator="$build_dir/generator"
boost="$build_dir/neural-boost"
prepare="$build_dir/network-prepare"
neural="$build_dir/neural"
#boost="./build-clang_release/src/lake/neural-boost"
#first="./neural-first"
ai_header="src/lake/NeuralNetworkData.hpp"
first_aifile="$2"
second_aifile="$3"
#best="./neural-best"

first="./neural-first"
second="./neural-second"

tup "$generator"
tup "$boost"
tup "$prepare"
$prepare "$first_aifile" "$ai_header"
tup "$neural"
cp "$neural" "$first"
$prepare "$second_aifile" "$ai_header"
tup "$neural"
cp "$neural" "$second"

better=0
worse=0
point_first=0
point_second=0

for cities in 100 1000 10000; do
    if [[ $cities == 10000 ]]; then
        ferries_list=10000
    else
        ferries_list="$cities $((cities * 2)) $((cities * 10))"
    fi
    for ferries in $ferries_list; do
        for min_cycle_time in 1 20; do
            for max_cycle_time in $((min_cycle_time * 10)) $((min_cycle_time * 50)); do
                for min_ferry_time in $min_cycle_time $((min_cycle_time * 20)); do
                    max_ferry_time=$((min_ferry_time * 10))
                    for time_limit in $((min_cycle_time * cities / 2)) $((min_cycle_time * cities * 2)); do
                        input=$($generator \
                                --cities=$cities \
                                --ferries=$ferries \
                                --min-cycle-time=$min_cycle_time \
                                --max-cycle-time=$max_cycle_time \
                                --min-ferry-time=$min_ferry_time \
                                --max-ferry-time=$max_ferry_time \
                                --time-limit=$time_limit)
                        if [ $? -ne 0 ]; then
                            echo "Failed."
                        else
                            #echo "$input" | timeout 2 "$boost" "$boost_aifile"
                            if ! echo "$input" | timeout 2 "$boost" "$first_aifile" >/dev/null 2>&1; then
                                continue
                            fi
                            firstoutput=$(echo "$input" | timeout 10 "$first" 2>&1 | tail -1)
                            if [ $? -ne 0 ]; then
                                echo "First did not finish in time"
                                continue
                            fi
                            #echo "First: $firstoutput"
                            firsttime=$(echo "$firstoutput" | cut -d ':' -f 3)
                            secondoutput=$(echo "$input" | timeout 10 "$second" 2>&1 | tail -1)
                            if [ $? -ne 0 ]; then
                                echo "Second did not finish in time"
                                continue
                            fi
                            #echo "Second: $secondoutput"
                            secondtime=$(echo "$secondoutput" | cut -d ':' -f 3)
                            point_first=$(($point_first + $firsttime * 100 / $time_limit))
                            point_second=$(($point_second + $secondtime * 100 / $time_limit))
                            difference=$(($point_second - $point_first))
                            if [ $secondtime -ge $firsttime ]; then
                                better=$((better + 1))
                                echo "BETTER! first $firsttime second: $secondtime limit: $time_limit pointdiff: $difference"
                            else
                                worse=$((worse + 1))
                                echo "WORSE! first: $firsttime second: $secondtime limit: $time_limit pointdiff: $difference"
                            fi
                        fi
                    done
                done
            done
        done
    done
done

echo "better: $better worse: $worse"
echo "first points: $point_first second points: $point_second"
