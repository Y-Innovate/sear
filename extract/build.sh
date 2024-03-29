#!/bin/bash

set -e

rm -rf *.o
rm -rf extract
rm -rf *.bin

as -mGOFF -I. -o irrseq00.o irrseq00.s || true
xlc -qascii -q64 -o extract profile_extract.c irrseq00.o
