#!/bin/bash
TIC=$(date +%s.%N)
./mcmf 1000.in
TOC=$(date +%s.%N)
echo -e $TOC-$TIC | bc >> time.txt
