#!/bin/bash

dir="samples/synthetic"
runs=10


make -j


echo "--- default ---"

dfile="$dir/default/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms50_dict100000_skew1.5"
qfile="${dfile}_10K_elems3-extent0.1%.qry"

 ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
 ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
 ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
 ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
 ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic


echo "--- vary_cardinality ---"

for card in "100000" "500000" "5000000" "10000000"; do

    dfile="$dir/vary_cardinality/norm_zipf_w128000000_n_dev1000000_a1.2_n${card}.txt_terms50_dict10000_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_domain ---"

for domain in "32000000" "64000000" "256000000" "512000000"; do

    dfile="$dir/vary_domain/norm_zipf_w${domain}_n_dev1000000_a1.2_n1000000.txt_terms50_dict100000_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_dictionary ---"

for dict in "10000" "50000" "500000" "1000000"; do

    dfile="$dir/vary_alpha/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms50_dict${dict}_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_alpha ---"

for alpha in "1.01" "1.1" "1.4" "1.8"; do

    dfile="$dir/vary_alpha/norm_zipf_w128000000_n_dev1000000_a${alpha}_n1000000.txt_terms50_dict100000_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_description ---"

for desc in "5" "10" "100" "500"; do

    dfile="$dir/vary_description/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms${desc}_dict100000_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_deviation ---"

for devi in "10000" "100000" "5000000" "10000000"; do

    dfile="$dir/vary_deviation/norm_zipf_w128000000_n_dev${devi}_a1.2_n1000000.txt_terms50_dict100000_skew1.5"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_skewness ---"

for skew in "1.001" "1.25" "1.75" "2.0"

    dfile="$dir/default/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms50_dict100000_skew${skew}"
    qfile="${dfile}_10K_elems3-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


echo "--- vary_qelems ---"

for elems in "1" "2" "4" "5"; do

    dfile="$dir/vary_qelems/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms50_dict100000_skew1.5"
    qfile="${dfile}_10K_elems${elems}-extent0.1%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done

echo "--- vary_qextent ---"

for extent in "0.01" "0.5" "1.0" "5.0"; do

    dfile="$dir/vary_qextent/norm_zipf_w128000000_n_dev1000000_a1.2_n1000000.txt_terms50_dict100000_skew1.5"
    qfile="${dfile}_10K_elems3-extent${extent}%.qry"

    ./query_irhint.exec -o A -m 13                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_irhint.exec -o B -m 15                   -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SHARDING -i 200 -x 2      -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o SLICING -p 250            -r $runs    $dfile $qfile       &>> output_synthetic
    ./query_tif.exec    -o HINTSLICING -p 250 -m 8   -r $runs    $dfile $qfile       &>> output_synthetic

done


make clean
