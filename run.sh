#!/usr/bin/env bash

res_dir="./meas"

mean_proc() {
    mcmd=$1
    iter=$2
    sum=0
    echo "$mcmd" >> log.log
    for _i in $(seq 1 "$iter"); do
        curr=$($mcmd)
        echo "$curr" >> log.log
        sum=$((sum  + curr))
    done
    mean=$(echo "scale=2; $sum / $iter" | bc)
    echo "$mean"
}

meas() {
    cmd=$1
    alg=$2
    thr=$3
    iter=100
    : > "$res_dir/$alg"
    for thr in $(seq 1 "$thr"); do
        echo "$alg" "$thr"
        fullcmd=$(echo "$cmd" | sed "s/%s/${thr}/")
        mean_proc "$fullcmd" "$iter" >> "$res_dir/$alg"
    done
}

openmp() {
    g++ openmp.cpp -o openmp -fopenmp
    meas "./openmp %s" "openmp" "$1"
}

mpi() {
    mpicxx mpi.cpp -o mpi
    meas "mpiexec -n %s mpi" "mpi" "$1"
}

# hybrid1() {
#     mpicxx hybrid.cpp -o hybrid -fopenmp
#     thr=$1
#     per=2
#     thr=$((thr / per))
#     meas "mpiexec -n %s hybrid $per" "hybrid1" "$thr"
# }
#
# hybrid2() {
#     mpicxx hybrid.cpp -o hybrid -fopenmp
#     thr=$1
#     per=8
#     thr=$((thr / per))
#     meas "mpiexec -n %s hybrid $per" "hybrid2" "$thr"
# }

run() {
    mkdir -p $res_dir
    max_thrs=$(lscpu | grep "Core(s)" | awk -F ':' '{print $2}' | xargs)
    # openmp "$max_thrs"
    mpi "$max_thrs"
    #hybrid1 "$max_thrs"
    #hybrid2 "$max_thrs"
}

run
