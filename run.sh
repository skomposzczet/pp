#!/usr/bin/env bash

res_dir="./meas"

meas() {
    cmd=$1
    alg=$2
    thr=$3
    : > "$res_dir/$alg"
    for thr in $(seq 1 "$thr"); do
        $cmd "$thr" >> "$res_dir/$alg"
    done
}

openmp() {
    g++ openmp.cpp -o openmp -fopenmp
    meas "./openmp" "openmp" "$1"
}

mpi() {
    mpicxx mpi.cpp -o mpi
    meas "mpiexec -np" "mpi" "$1"
}

run() {
    mkdir -p $res_dir
    max_thrs=$(lscpu | grep Core | awk -F ':' '{print $2}' | xargs)
    openmp "$max_thrs"
    # mpi "$max_thrs"
}

run
