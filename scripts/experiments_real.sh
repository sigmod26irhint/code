#!/bin/bash


make -j


echo "--- ECLOG (default) ---"

dfile="samples/eclog/ECOM-LOG.dat"

qfile="${dfile}_10K_elems3-extent0.1%.qry"

./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_eclog
./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_eclog
./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_eclog
./query_tif.exec    -o SHARDING -i 100 -x 5     -r 10 $dfile $qfile       &>> output_eclog
./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_eclog
./query_irhint.exec -o A -m 10                  -r 10 $dfile $qfile       &>> output_eclog
./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_eclog


echo "--- ECLOG (extent) ---"

for extent in "0.01" "0.05" "0.1" "0.5" "1.0"; do

    qfile="${dfile}_10K_elems3-extent${extent}%.qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SHARDING -i 100 -x 5     -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o A -m 10                  -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_eclog

done


echo "--- ECLOG (elements) ---"

for elems in "1" "2" "3" "4" "5"; do

    qfile="${dfile}_10K_elems${elems}-extent0.1%.qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SHARDING -i 100 -x 5     -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o A -m 10                  -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_eclog

done


echo "--- ECLOG (selectivity) ---"

for select in "select_10%-100%" "select_1%-10%" "select_0.1%-1%" "select_less-than-0.1%"; do

    qfile="${dfile}_10K_elems3-extent0.1%-elemid[$select].qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SHARDING -i 200 -x 2     -r 10 $dfile $qfile       &>> output_eclog
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o A -m 10                  -r 10 $dfile $qfile       &>> output_eclog
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_eclog

done


echo "--- WIKIPEDIA (default) ---"

dfile="samples/wikipedia/WIKIPEDIA-100K+_random-articles-all-revisions_[2020-2024).dat"

qfile="${dfile}_10K_elems3-extent0.1%.qry"

./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_wikipedia
./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_wikipedia
./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_wikipedia
./query_tif.exec    -o SHARDING -i 200 -x 2     -r 10 $dfile $qfile       &>> output_wikipedia
./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_wikipedia
./query_irhint.exec -o A -m 9                   -r 10 $dfile $qfile       &>> output_wikipedia
./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_wikipedia


echo "--- WIKIPEDIA (extent) ---"

for extent in "0.01" "0.05" "0.1" "0.5" "1.0"; do

    qfile="${dfile}_10K_elems3-extent${extent}%.qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SHARDING -i 200 -x 2     -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o A -m 9                   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_wikipedia

done


echo "--- WIKIPEDIA (elements) ---"

for elems in "1" "2" "3" "4" "5"; do

    qfile="${dfile}_10K_elems${elems}-extent0.1%.qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SHARDING -i 200 -x 2     -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o A -m 9                   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_wikipedia

done


echo "--- WIKIPEDIA (selectivity) ---"

for select in "select_10%-100%" "select_1%-10%" "select_0.1%-1%" "select_less-than-0.1%"; do

    qfile="${dfile}_10K_elems3-extent0.1%-elemid[$select].qry"

    ./query_tif.exec    -o HINTB -m 10              -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTG -m 5               -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o HINTSLICING -m 5 -p 50   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SHARDING -i 200 -x 2     -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_tif.exec    -o SLICING -p 50            -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o A -m 9                   -r 10 $dfile $qfile       &>> output_wikipedia
    ./query_irhint.exec -o B -m 10                  -r 10 $dfile $qfile       &>> output_wikipedia

done


echo "--- ECLOG (update) ---"

dfile_bottom="samples/eclog/ECOM-LOG_bottom90%.dat"

for percent in "1" "5" "10"; do

    dfile_top="samples/eclog/ECOM-LOG_top${percent}%.dat"

    ./update_tif.exec    -o HINTB -m 10              -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_tif.exec    -o HINTG -m 5               -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_tif.exec    -o HINTSLICING -m 5 -p 50   -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_tif.exec    -o SHARDING -i 200 -x 2     -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_tif.exec    -o SLICING -p 50            -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_irhint.exec -o A -m 10                  -d 206987  $dfile_bottom $dfile_top     &>> output_eclog
    ./update_irhint.exec -o B -m 10                  -d 206987  $dfile_bottom $dfile_top     &>> output_eclog

done


echo "--- WIKIPEDIA (update) ---"

dfile_bottom="samples/wikipedia/WIKIPEDIA-100K+_random-articles-all-revisions_[2020-2024)_bottom90%.dat"

for percent in "1" "5" "10"; do

    dfile_top="samples/wikipedia/WIKIPEDIA-100K+_random-articles-all-revisions_[2020-2024)_top${percent}%.dat"

    ./update_tif.exec    -o HINTB -m 10              -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_tif.exec    -o HINTG -m 5               -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_tif.exec    -o HINTSLICING -m 5 -p 50   -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_tif.exec    -o SHARDING -i 200 -x 2     -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_tif.exec    -o SLICING -p 50            -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_irhint.exec -o A -m 9                   -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia
    ./update_irhint.exec -o B -m 10                  -d 927283  $dfile_bottom $dfile_top     &>> output_wikipedia

done