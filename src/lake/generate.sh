#!/usr/bin/env bash

generator="$1"

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
                        file_name="c${cities}_f${ferries}_ct${min_cycle_time}-${max_cycle_time}_ft${min_ferry_time}-${max_ferry_time}_t${time_limit}.in"
                        echo "$file_name"
                        if ! timeout 1 "$generator" \
                                --cities=$cities \
                                --ferries=$ferries \
                                --min-cycle-time=$min_cycle_time \
                                --max-cycle-time=$max_cycle_time \
                                --min-ferry-time=$min_ferry_time \
                                --max-ferry-time=$max_ferry_time \
                                --time-limit=$time_limit \
                                >"$file_name"; then
                            rm -f "$file_name"
                            echo "Failed."
                        fi
                    done
                done
            done
        done
    done
done
