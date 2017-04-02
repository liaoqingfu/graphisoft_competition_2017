#!/usr/bin/env bash

for f in *.cpp; do
    chars=$(cat "$f" | tr -d '[:space:]' | wc -c)
    echo "$f: $chars"
done
