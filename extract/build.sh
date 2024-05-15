#!/bin/bash

set -e

rm -rf *.o
rm -rf extract
rm -rf *.bin

as -mGOFF -I. -o irrseq00.o irrseq00.s || true
ibm-clang++ -fzos-le-char-mode=ascii -I ./ -o extract \
    main.cpp \
    extract.cpp \
    post_process.cpp \
    irrseq00.o
