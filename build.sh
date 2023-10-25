#!/bin/bash

build_dir="debug-build"
build_type="-DCMAKE_BUILD_TYPE=Debug"

while test $# != 0
do
    case "$1" in
        --release|-r)
            build_dir="release-build"
            build_type="-DCMAKE_BUILD_TYPE=Release"
            ;;
        --debug|-d)
            build_dir="debug-build"
            build_type="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        --wsl)
            sh="powershell.exe"
            ext=".exe"
            ;;
        --run)
            run=true
            ;;
        --tidy)
            tidy=true
            ;;
        *)
            echo "unknown option $1"
            exit -1
            ;;
    esac
    shift
done

mkdir -p "$build_dir"

$sh cmake -G Ninja -S . -B "$build_dir" "$build_type"
$sh cmake --build "$build_dir"

[[ -n $tidy ]] && clang-tidy src/*.[ch]pp --header-filter=src/.* --checks=clang-analyzer-*,cppcoreguidelines-*,misc-*
[[ -n $run ]] && "$build_dir/R3$ext"

