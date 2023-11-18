#!/bin/bash

find engine/public -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find engine/private -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
find editor -iname *.[ch]pp -o -iname *.[ch] | xargs clang-format -i
