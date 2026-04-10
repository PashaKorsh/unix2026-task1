#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

make clean
make

# 1. Создать файл A.
./createA.sh A

# 2. Скопировать созданный файл A через нашу программу в файл B, сделав его разреженным: $ ./myprogram fileA fileB
./myprogram A B

# 3. Сжать A и B с помощью gzip
gzip -f -k A
gzip -f -k B

# 4. Распаковать сжатый файл B в stdout и сохранить через программу в файл C: $ gzip -cd fileB.gz | ./myprogram fileC
gzip -cd B.gz | ./myprogram C

# 5. Скопировать A через программу в файл D, указав нестандартный размер блока - 100 байт.
./myprogram -b 100 A D

{
    echo "Test 1: A == B by logic contents $(cmp -s A B && echo OK || echo FAIL)"
    echo

    echo "Test 2: A == C by logic contents $(cmp -s A C && echo OK || echo FAIL)"
    echo

    echo "Test 3: A == D by logic contents $(cmp -s A D && echo OK || echo FAIL)"
    echo

    # Программой stat вывести реальный размер файлов A, A.gz, B, B.gz, C, D
    echo "File size (logic size and physical size):"
    for f in A A.gz B B.gz C D; do
        if [ -e "$f" ]; then
            size=$(stat -c '%s' "$f")
            blocks=$(stat -c '%b' "$f")
            echo "$f: logic = ${size}B, physical = $((blocks * 512))B (${blocks} blocks 512B)"
        fi
    done
} > result.txt

echo "Done"
