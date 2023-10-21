#!/bin/bash

build_dir="debug-build"
build_type="-DCMAKE_BUILD_TYPE=Debug"
output_fmt="Ninja"
cmk="/usr/bin/cmake"
cwd=`pwd`

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
            cmk="/mnt/c/Program Files/CMake/bin/cmake.exe"
            cwd=`wslpath -w "$PWD"`
            # output_fmt="MinGW Makefiles"
            ;;
        --run)
            run=true
            ;;
        *)
            echo "unknown option $1"
            ;;
    esac
    shift
done

mkdir -p "$build_dir"

"$cmk" -G "$output_fmt" -S "$cwd" -B "$cwd/$build_dir" "$build_type"
"$cmk" --build "$build_dir"
[ -n $run ] && "$(pwd)/$build_dir/R3.exe"

