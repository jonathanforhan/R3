#!/bin/bash

find engine/src/api -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find engine/src/hal -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find engine/src/core -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find editor -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
