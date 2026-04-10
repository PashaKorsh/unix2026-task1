#!/usr/bin/env bash

file="${1:-A}"
size=$((4 * 1024 * 1024 + 1))

rm -f "$file"
dd if=/dev/zero of="$file" bs=4096 count=1024 status=none
printf '\1' | dd of="$file" bs=1 seek=0 conv=notrunc status=none
printf '\1' | dd of="$file" bs=1 seek=10000 conv=notrunc status=none
printf '\1' | dd of="$file" bs=1 seek=$((size-1)) conv=notrunc status=none
