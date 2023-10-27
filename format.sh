#!/bin/bash

find engine/api -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find engine/hal -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find engine/core -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find editor -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
